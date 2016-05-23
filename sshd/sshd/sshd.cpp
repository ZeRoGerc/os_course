//
//  main.cpp
//  sshd
//
//  Created by Vladislav Sazanovich on 22/05/16.
//  Copyright © 2016 ZeRoGerc. All rights reserved.
//

#include <iostream>
#include <vector>

using namespace std;

#define check(x) if ((x) == -1) {       \
perror(#x); \
exit(1); \
}

#define BUF_SIZE 1024

int main() {
    int pipe_in[2];
    int pipe_out[2];
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
    } else { // parent
        vector<char> buf(BUF_SIZE);
        size_t read_amount = 0;
        read_amount = read(STDIN_FILENO, buf.data(), BUF_SIZE);
        if (read_amount == 0) {
            exit(1);
        }
        while (read_amount > 0) {
            size_t remain = read_amount;
            while (remain != 0) {
                size_t delta = write(pipe_in[1], buf.data(), remain);
                check(delta);
                remain -= delta;
            }
            
            read_amount = read(STDIN_FILENO, buf.data(), BUF_SIZE);
        }
        
        read_amount = read(pipe_out[0], buf.data(), BUF_SIZE);
        while (read_amount > 0) {
            write(STDOUT_FILENO, buf.data(), read_amount);
            read_amount = read(STDIN_FILENO, buf.data(), BUF_SIZE);
        }
//        close(pipe_in[0]);
//        close(pipe_in[1]);
//        close(pipe_out[0]);
//        close(pipe_out[1]);
        
        int res;
        waitpid(id, &res, 0);
    }
}
