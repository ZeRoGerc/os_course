//
// Created by Vladislav Sazanovich on 03.01.16.
//

#ifndef SIMPLE_PROXY_PROXY_CLIENT_H
#define SIMPLE_PROXY_PROXY_CLIENT_H


#include <iosfwd>
#include <assert.h>
#include "socket.hpp"

struct proxy_client {
public:
    proxy_client(std::string const& ip, std::string const& host = "localhost", size_t port = 80);
    proxy_client(int descriptor);
    
    ~proxy_client();

    proxy_client(proxy_client const&) = delete;
    proxy_client& operator=(proxy_client const&) = delete;

    proxy_client(proxy_client&&) = delete;
    proxy_client& operator=(proxy_client&&) = delete;
    
    int get_socket() const {
        return client_socket.value();
    }
    
    std::string const& get_host() const {
        return host;
    }

    size_t send(std::string const& request);

    std::string read(size_t len);
    
    void init_terminal();
    
    std::pair<int, int> get_terminal_pipe() {
        assert(terminal_exists);
        return std::make_pair(pipe_out[0], pipe_in[1]);
    }
    
private:
    struct socket client_socket;
    std::string host = "localhost";
    
    int pipe_in[2];
    int pipe_out[2];
    bool terminal_exists;
};

#endif //SIMPLE_PROXY_PROXY_CLIENT_H
