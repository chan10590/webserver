#include "Socket.h"
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include "InetAddr.h"
#include "util.h"

namespace web {
Socket::Socket() : m_fd(-1) {
    m_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    errorExit(m_fd == -1, "socket create fail!");
}

Socket::Socket(int fd) : m_fd(fd) { errorExit(m_fd == -1, "socket create fail!"); }

Socket::~Socket() {
    if (m_fd != -1) {
        close(m_fd);
        m_fd = -1;
    }
}

/*设置端口复用*/
void Socket::setsockopt(int optval) {
    errorExit(::setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1,
              "reuse socket fail!");
}

/*绑定套接字*/
void Socket::bind(InetAddr* ipaddr) {
    errorExit(::bind(m_fd, (sockaddr*)&(ipaddr->m_addr), ipaddr->m_addr_len) == -1, "bind fail!");
}

/*监听套接字*/
void Socket::listen() { errorExit(::listen(m_fd, SOMAXCONN) == -1, "listen fail!"); }

/*接收客户端连接请求，建立连接，返回通信套接字*/
int Socket::accept(InetAddr* ipaddr) {
    int sfd = ::accept(m_fd, (sockaddr*)&(ipaddr->m_addr), &(ipaddr->m_addr_len));
    errorExit(sfd == -1, "accept fail!");
    return sfd;
}

void Socket::connect(InetAddr* ipaddr) {
    errorExit(::connect(m_fd, (sockaddr*)&(ipaddr->m_addr), ipaddr->m_addr_len) == -1,
              "socket connect fail!");
}

/*设置非阻塞*/
void Socket::setnonblocking() {
    int flag = fcntl(m_fd, F_GETFL, 0);
    flag |= O_NONBLOCK;
    errorExit(fcntl(m_fd, F_SETFL, flag) == -1, "nonblock fail!");
}

/*访问私有成员变量m_fd（监听套接字）的接口*/
int Socket::getFd() { return m_fd; }

}  // namespace web
