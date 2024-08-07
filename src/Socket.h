#ifndef WEB_SOCKET_H_
#define WEB_SOCKET_H_
#include "../util/noncopyable.h"
#include "InetAddr.h"

namespace web{
class Noncopyable;
class InetAddr;
/* 服务器的套接字的相关操作*/
class Socket : public Noncopyable{
public:
    Socket();
    ~Socket();

    void setsockopt(int optval = 1);
    void bind(const InetAddr& ipaddr);
    void listen();
    void setnonblocking(int fd);
    int accept(InetAddr& ipaddr);
    int getfd();
private:
    int m_fd;
};
}

#endif