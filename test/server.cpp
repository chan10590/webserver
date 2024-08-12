#include "../src/allsrc.h"
#include "../util/util.h"
#include <iostream>

int main(int argc, char* argv[]){
    if(argc < 2){
        std::cout << "input format: .out port" << std::endl;
    }
    uint16_t port = std::stoi(argv[1]);
    web::EventLoop* evloop = new web::EventLoop;
    web::Server server(evloop, port, EPOLLIN | EPOLLET);
    evloop->loop();
    return 0;
} 