#ifndef WEB_SOCKET_H_
#define WEB_SOCKET_H_

#include "../util/noncopyable.h"

namespace web {
    class Noncopyable;
    class InetAddr;
    /* 服务器的套接字的相关操作*/
    class Socket : public Noncopyable {
    private:
        int m_fd;
    public:
        Socket();
        Socket(int fd);
        ~Socket();

        void setsockopt(int optval = 1);
        void bind(InetAddr* ipaddr);
        void listen();
        int accept(InetAddr* ipaddr);
        void setnonblocking();
        int getFd();
    };
}

#endif