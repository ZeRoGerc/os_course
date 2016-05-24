//
// Created by Vladislav Sazanovich on 06.12.15.
//

#include <sys/socket.h>
#include <assert.h>
#include "proxy_client.hpp"
#include "event_queue.hpp"
#include "custom_exception.hpp"


#define check(x) if ((x) == -1) {       \
perror(#x); \
exit(1); \
}

#define BUF_SIZE 1024

int pipe_in[2];
int pipe_out[2];

void write_buf(int descriptor, std::vector<char> const& buf, size_t amount) {
    int wrote = 0;
    while (amount != 0) {
        size_t delta = write(descriptor, buf.data() + wrote, amount);
        amount -= delta;
        wrote += delta;
    }
}

event_queue::event_queue() {
    kq = kqueue();
}

event_queue::~event_queue() {}

void event_queue::register_server(int main_socket, handler register_function) {
    events_data[main_socket] = event_data();
    add_event(main_socket, EVFILT_READ, register_function);
}

void event_queue::add_client(proxy_client* client) {
    clients[client->get_socket()] = std::unique_ptr<proxy_client>(client);
    events_data[client->get_socket()] = event_data();
    
    client->init_terminal();
    int term = client->get_terminal_descriptor();
    
    add_event(term, EVFILT_READ, [this, client, term](struct kevent& event) {
        std::vector<char> buf(BUF_SIZE);
        size_t read_amount = read(term, buf.data(), BUF_SIZE);
        
        std::string& in_buffer = events_data[client->get_socket()].in_buffer;
        //TODO: faster
        for (int i = 0; i < read_amount; i++) {
            in_buffer += buf[i];
        }
        
        this->resume_write(client->get_socket());
    });
    
    add_event(term, EVFILT_WRITE, [this, client, term](struct kevent& event) {
        std::string& out_buffer = events_data[client->get_socket()].out_buffer;
        
        size_t write_amount = write(term, out_buffer.c_str(), out_buffer.size());
        check(write_amount);
        
        out_buffer = out_buffer.substr(write_amount);
        
        if (out_buffer.size() == 0) {
            this->stop_write(term);
        }
    });
    
    add_event(client->get_socket(), EVFILT_READ, [this](struct kevent& event) {
        this->handle_client_read(event);
    });
    
    add_event(client->get_socket(), EVFILT_WRITE, [this](struct kevent& event) {
        this->handle_client_write(event);
    });
    
    stop_write(client->get_socket());
    stop_write(term);
}

void event_queue::delete_event(size_t ident, int16_t filter) {
    event(ident, filter, EV_DELETE, 0, 0, nullptr);
}


void event_queue::add_event(size_t ident, int16_t filter, handler hand) {
    event(ident, filter, EV_ADD, 0, 0, hand);
}

int event_queue::occurred() {
    return kevent(kq, NULL, 0, evlist, SOMAXCONN, NULL);
}

bool event_queue::try_disconnect(struct kevent& event, int16_t filter) {
    if ((event.flags & EV_EOF) && (event.data == 0)) {
        proxy_client* client = clients[event.ident].get();
        if (client != nullptr) {
            delete_event(client->get_socket(), filter);
            
            clients.erase(event.ident);
            events_data.erase(event.ident);
        }
        return true;
    } else {
        return false;
    }
}

void event_queue::handle_client_read(struct kevent& event) {
    if (try_disconnect(event, EVFILT_READ)) {
        return;
    }
    proxy_client* client = clients[event.ident].get();
    event_data& data = events_data[event.ident];
    
    if (client == nullptr) {
        return;
    }
    data.out_buffer += client->read(BUF_SIZE);
    
    if (data.out_buffer.size() > 0) {
        resume_write(client->get_terminal_descriptor());
    }
}

void event_queue::handle_client_write(struct kevent& event) {
    if (try_disconnect(event, EVFILT_WRITE)) {
        return;
    }
    
    proxy_client* client = clients[event.ident].get();
    std::string& buf = events_data[event.ident].in_buffer;
    
    if (client != nullptr) {
        size_t sent = client->send(buf);
        buf = buf.substr(sent);
        
        if (buf.size() == 0) {
            stop_write(client->get_socket());
        }
    }
}

void event_queue::resume_write(size_t fd) {
    handler hand = events_data[fd].write_function;
    add_event(fd, EVFILT_WRITE, hand);
}

void event_queue::stop_write(size_t fd) {
    delete_event(fd, EVFILT_WRITE);
}


void event_queue::execute(int main_server, int amount) {
    for (int i = 0; i < amount; i++) {
        struct kevent& event = evlist[i];
        
        if (event.filter == EVFILT_READ) {
            events_data[event.ident].read_function(event);
            continue;
        }
        
        if (event.filter == EVFILT_WRITE) {
            events_data[event.ident].write_function(event);
            continue;
        }
        
    }
}

void event_queue::event(size_t ident, int16_t filter, uint16_t flags, uint32_t fflags, int64_t data, handler hand) {
    struct kevent temp_event;
    
    if (hand != nullptr) {
        event_data& additional = events_data[ident];
        if (filter == EVFILT_READ) {
            additional.read_function = hand;
        }
        if (filter == EVFILT_WRITE) {
            additional.write_function = hand;
        }
    }
    
    EV_SET(&temp_event, ident, filter, flags, fflags, data, nullptr);

    if (kevent(kq, &temp_event, 1, NULL, 0, NULL) == -1) {
        std::string message{"kevent fails: "};
        message.append(std::strerror(errno));
        throw custom_exception(message);
    }
}
