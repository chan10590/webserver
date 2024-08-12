#include "../util/util.h"
#include "Server.h"
#include "Socket.h"
#include "Channel.h"
#include <iostream>
#include <functional>
#include <unistd.h>

namespace web {
Server::Server(EventLoop* evloop, uint16_t port, uint32_t ev) : m_evloop(evloop), m_port(port), m_event(ev) {
    // 创建监听套接字
    Socket* serv_sock = new Socket;//内存泄漏
    serv_sock->setsockopt();
    InetAddr* serv_addr = new InetAddr("127.0.0.1", m_port);//内存泄漏
    serv_sock->bind(serv_addr);
    serv_sock->listen();
    int lfd = serv_sock->getFd();
    setNonblocking(lfd);// 非阻塞

    Channel* serv_channel = new Channel(m_evloop, serv_sock->getFd());
    std::function<void()> connect_cb = std::bind(&Server::newConnection, this, serv_sock);
    serv_channel->setCallback(connect_cb);
    serv_channel->setAddChannel(m_event);
}

void Server::newConnection(Socket* sock) {
    InetAddr* clnt_addr = new InetAddr;//内存泄漏，智能指针管理
    int sfd = sock->accept(clnt_addr);
    setNonblocking(sfd);// 非阻塞
    std::cout << "client " << sfd << " connect!" << std::endl;
    // epoll监听通信套接字的读事件
    Channel* new_ch = new Channel(m_evloop, sfd);//内存泄漏，智能指针管理
    std::function<void()> commun_cb = std::bind(&Server::handleReadCommun, this, sfd);
    new_ch->setCallback(commun_cb);
    new_ch->setAddChannel(m_event);
}

void Server::handleReadCommun(int fd) {
    char* msg = const_cast<char* >("hello\n");
    char buf[1024];
    while(1){
        memset(buf, 0, sizeof(buf));
        int ret = read(fd, buf, sizeof(buf));
        // 读取到通信请求并响应
        if(ret > 0){
            std::cout << "client " << fd << " send: " << buf << std::endl;
            write(fd, msg, strlen(msg));
        }
        // 客户端断开连接，需要从epoll上移除相应的套接字及事件
        else if(ret == 0){
            std::cout << "client " << fd << " break connection!" << std::endl;
            //web::errorExit(epoll_ctl(epfd,EPOLL_CTL_DEL,evi_fd,&trig_evs[i])==-1,"epoll delete communication socket fail!");
            close(fd);
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
}