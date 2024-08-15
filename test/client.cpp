#include <string.h>
#include <unistd.h>
#include <iostream>
#include "../src/inc/InetAddr.h"
#include "../src/inc/Socket.h"
#include "../src/inc/util.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "input format: .out port" << std::endl;
    }
    uint16_t port = std::stoi(argv[1]);
    web::Socket* sock = new web::Socket();
    web::InetAddr* addr = new web::InetAddr("127.0.0.1", port);
    sock->connect(addr);
    int cfd = sock->getFd();

    while (1) {
        char buf[1024];
        std::cout << "input: " << std::endl;
        std::cin >> buf;
        write(cfd, buf, strlen(buf));
        int ret = read(cfd, buf, sizeof(buf));
        if (ret == 0) {
            std::cout << "server closed!" << std::endl;
            break;
        } else if (ret > 0) {
            std::cout << "client received " << buf << std::endl;
        } else {
            close(cfd);
            web::errorExit(true, "read fail!");
        }
    }
    delete addr;
    delete sock;
    return 0;
}
