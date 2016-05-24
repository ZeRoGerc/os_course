//
//  main.cpp
//  sshd
//
//  Created by Vladislav Sazanovich on 22/05/16.
//  Copyright © 2016 ZeRoGerc. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <vector>
#include "event_queue.hpp"
#include "proxy.hpp"
#include <unistd.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <cstdio>

using namespace std;

#define check(x) if ((x) == -1) {       \
perror(#x); \
exit(1); \
}

const std::string file = "/tmp/rshd.pid";

void sig_handler(int signum, siginfo_t *info, void *context)
{
    if (signum == SIGTERM || signum == SIGINT) {
        if(remove(file.c_str()) != 0) {
            perror("Error deleting file");
        }
    }
}

int main(int argc, char *argv[]) {
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGTERM);
    sigaddset(&act.sa_mask, SIGINT);
    act.sa_sigaction = &sig_handler;
    act.sa_flags = SA_SIGINFO;
    
    if ((sigaction(SIGTERM, &act, NULL) < 0) || (sigaction(SIGINT, &act, NULL) < 0))
    {
        perror("sigaction failed");
        return errno;
    }
    
    std::ifstream stream(file);
    if (stream) {
        printf("Daemon already running\n");
        exit(1);
    }
    stream.close();
    
    pid_t pid = fork();
    check(pid);
    
    if (pid != 0) {
        exit(0);
    }
    
    pid_t sid = setsid();
    check(sid);

    pid = fork();
    check(pid);
    
    if (pid != 0) {
        std::ofstream o(file, std::ostream::trunc);
        o << pid;
        o.close();
        exit(0);
    }
    
    uint16_t port = 2541;
    if (argc > 1) {
        port = (uint16_t) atoi(argv[1]);
    }
    
    event_queue kq;
    proxy proxy_server{port, &kq};
    proxy_server.main_loop();
}
