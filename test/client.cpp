#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>
#include <string.h>
#include <fcntl.h>
#include "../util/util.h"

int main(int argc, char* argv[]){
    if(argc < 2){
        std::cout << "input format: .out port" << std::endl;
    }
    uint16_t port = std::stoi(argv[1]);
    int cfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    web::errorExit(cfd<0, "socket fail!");
    struct sockaddr_in servsock;
    memset(&servsock,0,sizeof(servsock));
    servsock.sin_family = AF_INET;
    inet_pton(AF_INET,"127.0.0.1",&servsock.sin_addr.s_addr);
    servsock.sin_port = htons(port);
    web::errorExit(connect(cfd, reinterpret_cast<sockaddr*>(&servsock),static_cast<socklen_t>(sizeof(servsock))) < 0, "connect fail!");
    while(1){
        char buf[4096];
        std::cout << "input: "<< std::endl;
        std::cin >> buf;
        write(cfd,buf,strlen(buf));
        int ret = read(cfd,buf,sizeof(buf));
        if(ret == 0){
            std::cout << "server closed!" << std::endl;
            break;
        }
        else if(ret>0){
            std::cout << "client received " << buf << std::endl;
        }else{
            close(cfd);
            web::errorExit(true, "read fail!");
        }

    }
    close(cfd);
    return 0;
}