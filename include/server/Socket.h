#ifndef SOCKET_H_
#define SOCKET_H_

#include <cstdint>
#include <string>
#include "noncopyable.h"

/* 服务器的套接字的相关操作*/
class Socket : public Noncopyable {
 public:
    Socket();
    explicit Socket(int fd);
    ~Socket();

    void setsockopt(int optval = 1) const;
    void setnonblocking() const;

    void create();
    void bind(const char* ip, uint16_t port) const;
    void listen() const;
    void accept(int& fd) const;

    void connect(const char* ip, uint16_t port) const;

    int getFd() const;
    void setFd(int fd);
    std::string getAddr() const;

 private:
    int _fd;
};

#endif  // END SOCKET_H_
