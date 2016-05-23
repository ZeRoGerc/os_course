//
// Created by Vladislav Sazanovich on 03.01.16.
//

#include "proxy_client.hpp"
#include <sys/socket.h>
#include <vector>

#define check(x) if ((x) == -1) {       \
perror(#x); \
exit(1); \
}

proxy_client::proxy_client(std::string const& ip, std::string const& host, size_t port)
: client_socket(ip, port), host(host) {}


proxy_client::proxy_client(int descriptor)
        : client_socket(descriptor) {}

proxy_client::~proxy_client() {
    if (terminal_exists) {
        close(pipe_in[0]);
        close(pipe_in[1]);
        close(pipe_out[0]);
        close(pipe_out[1]);
    }
}

size_t proxy_client::send(std::string const& request) {
    if (request.size() == 0) return 0;  
    ssize_t len = ::send(get_socket(), request.c_str(), request.size(), 0);

    if (len == -1) len = 0;
    return static_cast<size_t>(len);
}

std::string proxy_client::read(size_t len) {
    std::vector<char> buf(len);
    ssize_t new_len = ::recv(get_socket(), buf.data(), len, 0);
    if (new_len == -1) {
        return "";
    }
    
    return std::string(buf.begin(), buf.begin() + new_len);
}


void proxy_client::init_terminal() {
    terminal_exists = true;
    
    check(pipe(pipe_in));
    check(pipe(pipe_out));
    
    pid_t id = fork();
    
    if (id == 0) { //child
        dup2(pipe_in[0], STDIN_FILENO);
        dup2(pipe_out[1], STDOUT_FILENO);
        close(pipe_in[0]);
        close(pipe_in[1]);
        close(pipe_out[0]);
        close(pipe_out[1]);
        execlp("/bin/sh", "/bin/sh", NULL);
    }
}