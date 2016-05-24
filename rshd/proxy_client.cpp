//
// Created by Vladislav Sazanovich on 03.01.16.
//

#include "proxy_client.hpp"
#include <sys/socket.h>
#include <vector>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define _XOPEN_SOURCE 600

#define check(x) if ((x) == -1) {       \
perror(#x); \
exit(1); \
}

proxy_client::proxy_client(std::string const& ip, std::string const& host, size_t port)
: client_socket(ip, port), host(host) {}


proxy_client::proxy_client(int descriptor)
        : client_socket(descriptor) {}

proxy_client::~proxy_client() {}

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
    
    int master_fd = posix_openpt(O_RDWR);
    check(master_fd);
    check(grantpt(master_fd));
    check(unlockpt(master_fd));
    
    int slave_fd = open(ptsname(master_fd), O_RDWR);
    check(slave_fd);
    
    pid_t id = fork();
    
    if (id == 0) { //child
        struct termios tattr;
        
        tcgetattr (slave_fd, &tattr);
        tattr.c_lflag &= ~(ICANON | ECHO);
        tattr.c_cc[VMIN] = 1;
        tattr.c_cc[VTIME] = 0;
        tcsetattr (slave_fd, TCSANOW, &tattr);
        
        
        dup2(slave_fd, STDIN_FILENO);
        dup2(slave_fd, STDOUT_FILENO);
        dup2(slave_fd, STDERR_FILENO);
        close(master_fd);
        close(slave_fd);
        
        setsid();
        ioctl(0, TIOCSCTTY, 1);
        
        execlp("/bin/sh", "/bin/sh", NULL);
    }
    close(slave_fd);
    terminal_fd = master_fd;
}