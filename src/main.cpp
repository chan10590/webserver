#include "InetAddr.h"
#include "Socket.h"
#include "Epoll.h"
#include <iostream>
#include <vector>
#include <unistd.h>

static char* msg = const_cast<char*>("hello\n");

void handle_commun(int evi_fd){
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
            //web::errorExit(epoll_ctl(epfd,EPOLL_CTL_DEL,evi_fd,&trig_evs[i])==-1,"epoll delete communication socket fail!");
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

int main(int argc, char* argv[]){
    if(argc < 2){
        std::cout << "input format: .out port" << std::endl;
    }
    uint16_t port = std::stoi(argv[1]);
    // 创建监听套接字
    web::Socket sock;
    sock.setsockopt();
    web::InetAddr sock_addr("127.0.0.1", port);
    sock.bind(sock_addr);
    sock.listen();
    // 创建epoll
    web::Epoll ep;
    // epoll监听监听套接字的读事件(边沿触发)
    int lfd = sock.getfd();
    ep.addNode(lfd, EPOLLIN | EPOLLET);
    sock.setnonblocking(lfd);// 非阻塞
    // epoll循环监听
    while(1){
        std::vector<epoll_event*> trig_evptr = ep.triggerNodeSet();
        int trig_evnum = trig_evptr.size();
        // 处理触发事件集合
        for(int i = 0; i < trig_evnum; ++i){
            // 监听套接字发生读事件，说明有客户端请求建立连接
            int evi_fd = (*trig_evptr[i]).data.fd;
            if(evi_fd == sock.getfd()){
                // 建立连接，产生通信套接字
                int sfd = sock.accept(sock_addr);
                // epoll监听通信套接字的读事件
                ep.addNode(sfd, EPOLLIN | EPOLLET);
                sock.setnonblocking(sfd);
            }
            // 通信套接字发生读事件
            else if((*trig_evptr[i]).events & EPOLLIN){
                handle_commun(evi_fd);
            }
            // 通信套接字发生其他事件
            else{}
        }
    }
    return 0;
}