#include "Socket.h"
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include "../util/util.h"

namespace web{
Socket::Socket(): m_fd(-1){
    m_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    web::errorExit(m_fd == -1, "socket create fail!");
}

Socket::~Socket(){
    if(m_fd != -1){
        close(m_fd);
        m_fd = -1;
    }
}

/*设置端口复用*/
void Socket::setsockopt(int optval){
    errorExit(::setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1, "reuse socket fail!");
}
/*绑定套接字*/
void Socket::bind(const InetAddr& ipaddr){
    errorExit(::bind(m_fd, (sockaddr*)&(ipaddr.m_addr),ipaddr.m_addr_len) == -1, "bind fail!");
}
/*监听套接字*/
void Socket::listen(){
    errorExit(::listen(m_fd, SOMAXCONN) == -1, "listen fail!");
}
/*设置非阻塞*/
void Socket::setnonblocking(int fd){
    int flag = fcntl(fd, F_GETFL, 0);
    flag |= O_NONBLOCK;
    errorExit(fcntl(fd, F_SETFL, flag) == -1, "nonblock fail!");
}
/*接收客户端连接请求，建立连接，返回通信套接字*/
int Socket::accept(InetAddr& ipaddr){
    int sfd = ::accept(m_fd,(sockaddr*)&(ipaddr.m_addr),&(ipaddr.m_addr_len));
    errorExit(sfd == -1, "accept fail!");
    return sfd;
}
/*访问私有成员变量m_fd（监听套接字）的接口*/
int Socket::getfd(){
    return m_fd;
}
}
