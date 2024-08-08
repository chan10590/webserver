#include "../src/InetAddr.h"
#include "../src/Socket.h"
#include "../src/Epoll.h"
#include "../src/Channel.h"
#include "../util/util.h"
#include <iostream>
#include <vector>
#include <unistd.h>
#include <memory>

static char* msg = const_cast<char*>("hello\n");

/*处理通信请求*/
void handle_commun(int evi_fd){
    char buf[1024];
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
    web::Socket serv_sock;
    serv_sock.setsockopt();
    web::InetAddr serv_addr("127.0.0.1", port);
    serv_sock.bind(serv_addr);
    serv_sock.listen();
    int lfd = serv_sock.getFd();
    web::setNonblocking(lfd);// 非阻塞
    // 创建epoll
    web::Epoll ep;
    // epoll监听监听套接字的读事件(边沿触发)
    web::Channel ch(&ep, lfd);
    ch.setAddChannel(EPOLLIN | EPOLLET);
    // epoll循环监听
    while(1){
        std::vector<web::Channel *> trig_evptr = ep.triggerNodeSet();
        int trig_evnum = trig_evptr.size();
        std::cout << trig_evnum << " events happened!" << std::endl;
        // 处理触发事件集合
        for(int i = 0; i < trig_evnum; ++i){
            // 监听套接字发生读事件，说明有客户端请求建立连接
            int evi_fd = trig_evptr[i]->getFd();
            if(evi_fd == lfd){
                // 建立连接，产生通信套接字
                web::InetAddr* clnt_addr = new web::InetAddr;//内存泄漏，智能指针管理
                int sfd = serv_sock.accept(*clnt_addr);
                web::setNonblocking(sfd);// 非阻塞
                std::cout << "client " << sfd << " connect!" << std::endl;
                // epoll监听通信套接字的读事件
                web::Channel* new_ch = new web::Channel(&ep, sfd);//内存泄漏，智能指针管理
                new_ch->setAddChannel(EPOLLIN | EPOLLET);
            }
            // 通信套接字发生读事件
            else if(trig_evptr[i]->getRevents() & EPOLLIN){
                std::cout << "client " << evi_fd << " request communication!" << std::endl; 
                handle_commun(evi_fd);
            }
            // 通信套接字发生其他事件
            else{}
        }
        std::cout << "------------------------------------" << std::endl;
    }
    return 0;
}