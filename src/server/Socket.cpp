#include "Socket.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cassert>
#include <cstring>
#include "Logger.h"
#include "util.h"

// 构造Socket对象
Socket::Socket() : _fd(-1) {}
Socket::~Socket() {
    LOG_INFO("Socket deconstrutor");
    if (_fd != -1) {
        close(_fd);
    }
}

// 设置端口复用
void Socket::setsockopt(int optval) const {
    errorExit(::setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1, "reuse socket fail!");
}
// 设置非阻塞
void Socket::setnonblocking() const {
    int flag = fcntl(_fd, F_GETFL, 0);
    flag |= O_NONBLOCK;
    errorExit(fcntl(_fd, F_SETFL, flag) == -1, "nonblock fail!");
}

// 创建套接字
void Socket::create() {
    assert(_fd == -1);
    _fd = ::socket(AF_INET, SOCK_STREAM, 0);
    errorExit(_fd == -1, "socket create fail!");
}
// 绑定套接字
void Socket::bind(const char* ip, uint16_t port) const {
    assert(_fd != -1);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    errorExit(::bind(_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1, "bind fail!");
}
// 监听套接字
void Socket::listen() const { errorExit(::listen(_fd, SOMAXCONN) == -1, "listen fail!"); }
// 与客户端建立连接
void Socket::accept(int& fd) const {
    assert(_fd != -1);
    fd = ::accept(_fd, nullptr, nullptr);
    errorExit(fd == -1, "accept fail!");
}

// 与服务端建立连接
void Socket::connect(const char* ip, uint16_t port) const {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    errorExit(::connect(_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1, "socket connect fail!");
}

// 返回文件描述符
int Socket::getFd() const { return _fd; }
// 设置文件描述符
void Socket::setFd(int fd) { _fd = fd; }
// 返回对端地址
std::string Socket::getAddr() const {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    socklen_t len = sizeof(addr);
    if (getpeername(_fd, (struct sockaddr*)&addr, &len) == -1) {
        return "";
    }
    char ip[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip)) == nullptr) {
        return "";
    }
    std::string ret(ip);
    ret += ":";
    ret += std::to_string(ntohs(addr.sin_port));
    return ret;
}
