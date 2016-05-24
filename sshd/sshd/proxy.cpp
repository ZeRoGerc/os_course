//
// Created by Vladislav Sazanovich on 06.12.15.
//

#include "event_queue.hpp"
#include "proxy.hpp"
#include "proxy_client.hpp"
#include <memory>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <arpa/inet.h>
#include <exception>
#include <sys/fcntl.h>
#include <iostream>


main_server::main_server(int port) : port(port) {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    const int set = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, &set, sizeof(set)) == -1) {
        throw custom_exception("fail to reuse port");
    };
    
    sockaddr_in server;
    
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;
    
    int flags;
    if (-1 == (flags = fcntl(server_socket, F_GETFL, 0))) {
        flags = 0;
    }
    if (fcntl(server_socket, F_SETFL, flags | O_NONBLOCK) == -1) {
        throw custom_exception("fail to create connect socket");
    }
    
    int bnd = bind(server_socket, (struct sockaddr*)&server, sizeof(server));
    if (bnd == -1) {
        throw custom_exception("fail to bind connect socket");
    }
    if (listen(server_socket, SOMAXCONN) == -1) {
        throw custom_exception("fail to listen connect socket");
    }
}

proxy::proxy(int main_socket, event_queue* queue)
: queue(queue)
, connect_server(main_server{main_socket})
{
    
    queue->register_server(connect_server.get_socket(), [this](struct kevent& event) {
        proxy_client* client = new proxy_client(this->connect_server.get_socket());
        this->queue->add_client(client);
    });
}

proxy::~proxy() {
}

void proxy::main_loop() {
    while (work) {
        if (int amount = queue->occurred()) {
            queue->execute(connect_server.get_socket(), amount);
        }
    }
}