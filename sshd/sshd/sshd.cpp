//
//  main.cpp
//  sshd
//
//  Created by Vladislav Sazanovich on 22/05/16.
//  Copyright © 2016 ZeRoGerc. All rights reserved.
//

#include <iostream>
#include <vector>
#include "event_queue.hpp"
#include "proxy.hpp"

using namespace std;

int main() {
    event_queue kq;
    proxy proxy_server{&kq};
    
    proxy_server.main_loop();
}
