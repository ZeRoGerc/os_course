//
//  event_queue.hpp
//  simple_proxy
//
//  Created by Vladislav Sazanovich on 28.11.15.
//  Copyright © 2015 ZeRoGerc. All rights reserved.
//

#ifndef event_queue_hpp
#define event_queue_hpp

#include "proxy_client.hpp"
#include <stdio.h>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <sys/socket.h>
#include <functional>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <set>
#include <map>
#include <array>
#include <thread>
#include <queue>

using handler = std::function<void(struct kevent&)>;

struct event_data {
    handler read_function;
    handler write_function;
    
    std::string in_buffer;
    std::string out_buffer;
};

struct event_queue {
public:
    event_queue();
    
    ~event_queue();

    void register_server(int main_socket, handler register_function);
    
    void add_client(proxy_client* client);
    
    void delete_event(size_t ident, int16_t filter);

    void add_event(size_t ident, int16_t filter, handler hand);
    
    void event(size_t ident, int16_t filter, uint16_t flags, uint32_t fflags, int64_t data, handler hand);
    
    int occurred();

    void execute(int main_server, int amount);
    
    void stop_resolve();
private:
    struct kevent evlist[1024];
    int kq;
    
    std::map<uintptr_t, std::unique_ptr<proxy_client>> clients;
    std::map<uintptr_t, event_data> events_data;
    
    void handle_client_read(struct kevent& event);
    void handle_client_write(struct kevent& event);
    bool try_disconnect(struct kevent& event, int16_t filter);
    
    void resume_write(size_t fd);
    void stop_write(size_t fd);
};

#endif /* event_queue_hpp */
