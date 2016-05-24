//
//  proxy.hpp
//  simple_proxy
//
//  Created by Vladislav Sazanovich on 28.11.15.
//  Copyright © 2015 ZeRoGerc. All rights reserved.
//

#ifndef proxy_hpp
#define proxy_hpp

#include <stdio.h>
#include <vector>
#include <utility>
#include "event_queue.hpp"
#include "custom_exception.hpp"

struct main_server {
public:
    main_server(int port);
    
    main_server& operator=(main_server const&) = delete;
    main_server(main_server const&) = delete;
    
    main_server& operator=(main_server&&) = default;
    main_server(main_server&&) = default;
    
    ~main_server() {
        close(server_socket);
    }
    
    int get_socket() {
        return server_socket;
    }
private:
    int server_socket;
    int port;
};


struct proxy {
public:
    proxy(int main_socket, event_queue* queue);
    ~proxy();
    
    proxy(proxy const&) = delete;
    proxy& operator=(proxy const&) = delete;
    
    proxy(proxy&&) = delete;
    proxy& operator=(proxy&&) = delete;
    
    void main_loop();
private:
    main_server connect_server;
    event_queue* queue;
    
    bool work = true;
};

#endif /* proxy_hpp */
