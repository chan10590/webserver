#include <iostream>
#include "../src/inc/allsrc.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "input format: .out port" << std::endl;
    }
    uint16_t port = std::stoi(argv[1]);
    web::EventLoop* evloop = new web::EventLoop;
    web::Server* server = new web::Server(evloop, port, EPOLLIN | EPOLLPRI);
    evloop->loop();
    delete evloop;
    delete server;
    return 0;
}
