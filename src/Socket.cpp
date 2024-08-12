#include "../util/util.h"
#include "Socket.h"
#include <sys/socket.h>
#include <fcntl.h>

namespace web{
    /*设置端口复用*/
    void Socket::setsockopt(int optval) {
        errorExit(::setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1, "reuse socket fail!");
    }

    /*绑定套接字*/
    void Socket::bind(InetAddr* ipaddr) {
        errorExit(::bind(m_fd, (sockaddr*)&(ipaddr->m_addr), ipaddr->m_addr_len) == -1, "bind fail!");
    }

    /*监听套接字*/
    void Socket::listen(){
        errorExit(::listen(m_fd, SOMAXCONN) == -1, "listen fail!");
    }

    /*接收客户端连接请求，建立连接，返回通信套接字*/
    int Socket::accept(InetAddr* ipaddr) {
        int sfd = ::accept(m_fd, (sockaddr*)&(ipaddr->m_addr), &(ipaddr->m_addr_len));
        errorExit(sfd == -1, "accept fail!");
        return sfd;
    }

    /*访问私有成员变量m_fd（监听套接字）的接口*/
    int Socket::getFd() {
        return m_fd;
    }

}
