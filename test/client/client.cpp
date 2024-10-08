#include <string.h>
#include <unistd.h>
#include <iostream>
#include "InetAddr.h"
#include "Socket.h"
#include "ThreadPool.h"
#include "util.h"

void client(uint16_t port) {
    Socket* sock = new Socket();
    InetAddr* addr = new InetAddr("127.0.0.1", port);
    sock->connect(addr);
    int cfd = sock->getFd();

    while (1) {
        char buf[1024] = {0};
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
            errorExit(true, "read fail!");
        }
    }
    delete addr;
    delete sock;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "input format: .out port" << std::endl;
    }
    uint16_t port = std::stoi(argv[1]);
    int th_num = 5;
    ThreadPool* thpool = new ThreadPool(th_num);
    std::function<void()> task = std::bind(client, port);
    for (int i = 0; i < th_num; ++i) {
        thpool->addTask(task);
    }
    delete thpool;
    return 0;
}
