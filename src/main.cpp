#include <iostream>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include "../util/util.h"
#include <sys/epoll.h>

#define MAXLISTEN 128
#define MAXEVENTS 1024


int main(int argc, char* argv[]){
    if(argc < 2){
        std::cout << "input format: .out port" << std::endl;
    }
    uint16_t port = std::stoi(argv[1]);
    char* msg = const_cast<char*>("hello\n");
    // 创建监听套接字
    int lfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    web::errorExit(lfd<0, "socket fail!");
    int optval = 1;
    web::errorExit(setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1, "reuse socket fail!");
    struct sockaddr_in servsock;
    memset(&servsock,0,sizeof(servsock));
    servsock.sin_family = AF_INET;
    servsock.sin_addr.s_addr = htonl(INADDR_ANY);
    servsock.sin_port = htons(port);
    web::errorExit(bind(lfd, reinterpret_cast<sockaddr*>(&servsock ),static_cast<socklen_t>(sizeof(servsock))) < 0, "bind fail!");
    web::errorExit(listen(lfd, MAXLISTEN) < 0, "listen fail!");
    // 创建epoll
    int epfd = epoll_create1(0);
    web::errorExit(epfd==-1, "epoll create fail!");
    // epoll监听监听套接字的读事件(边沿触发)
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data.fd = lfd;
    ev.events = EPOLLIN | EPOLLET;
    web::setNonBlock(lfd);// 非阻塞
    web::errorExit(epoll_ctl(epfd,EPOLL_CTL_ADD,lfd,&ev)==-1,"epoll add listen socket fail!");
    // 存放已触发事件的事件集合
    struct epoll_event trig_evs[MAXEVENTS];
    memset(&trig_evs, 0, sizeof(trig_evs));
    // epoll循环监听
    while(1){
        int trig_evnum = epoll_wait(epfd, trig_evs, MAXEVENTS, -1);
        web::errorExit(trig_evnum == -1, "epoll wait fail!");
        // 处理触发事件集合
        for(int i = 0; i < trig_evnum; ++i){
            // 监听套接字发生读事件，说明有客户端请求建立连接
            int evi_fd = trig_evs[i].data.fd;
            if(evi_fd == lfd){
                // 建立连接，产生通信套接字
                struct sockaddr_in clntsock;
                memset(&clntsock,0,sizeof(clntsock));
                socklen_t clntlen = static_cast<socklen_t>(sizeof(clntsock));
                int sfd = accept(lfd,reinterpret_cast<sockaddr*>(&clntsock), &clntlen);
                web::errorExit(sfd<0, "accept fail!");
                // epoll监听通信套接字的读事件
                memset(&ev, 0, sizeof(ev));
                ev.data.fd = sfd;
                ev.events = EPOLLIN | EPOLLET;
                web::setNonBlock(sfd);// 非阻塞
                web::errorExit(epoll_ctl(epfd,EPOLL_CTL_ADD,sfd,&ev)==-1,"epoll add communication socket fail!");
            }
            // 通信套接字发生读事件
            else if(trig_evs[i].events & EPOLLIN){
                // 处理通信请求
                char buf[4096];
                while(1){
                    memset(buf,0,sizeof(buf));
                    int ret = read(evi_fd,buf,sizeof(buf));
                    // 读取到通信请求并响应
                    if(ret > 0){
                        std::cout << "client " << evi_fd << " send: " << buf << std::endl;
                        write(evi_fd,msg,strlen(msg));
                    }
                    // 客户端断开连接，需要从epoll上移除相应的套接字及事件
                    else if(ret == 0){
                        std::cout << "client " << evi_fd << " break connection!" << std::endl;
                        web::errorExit(epoll_ctl(epfd,EPOLL_CTL_DEL,evi_fd,&trig_evs[i])==-1,"epoll delete communication socket fail!");
                        close(evi_fd);
                        break; 
                    }
                    // 正常中断，再次尝试读取
                    else if(ret == -1 && errno == EINTR)
                    {
                        std::cout << "normal interpret! read again!" << std::endl;
                        continue;
                    }
                    // 对于非阻塞IO，已经读完数据，没有数据可读取
                    else if(ret == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
                    {
                        std::cout << "read all datas! no readable data!" << std::endl;
                        break;
                    }
                }
            }
            // 通信套接字发生其他事件
            else{
                
            }
        }
    }
    close(lfd);
    return 0;
}