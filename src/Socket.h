#ifndef WEB_SOCKET_H_
#define WEB_SOCKET_H_

#include "../util/noncopyable.h"
#include "InetAddr.h"
#include <unistd.h>

namespace web {
    class Noncopyable;
    class InetAddr;
    /* 服务器的套接字的相关操作*/
    class Socket : public Noncopyable {
    private:
        int m_fd;
    public:
        Socket() : m_fd(-1) {
            m_fd = ::socket(AF_INET, SOCK_STREAM, 0);
            errorExit(m_fd == -1, "socket create fail!");
        }
        Socket(int fd) : m_fd(fd) {
            errorExit(m_fd == -1, "socket create fail!");
        }
        ~Socket() {
            if(m_fd != -1) {
                close(m_fd);
                m_fd = -1;
            }
        }

        void setsockopt(int optval = 1);
        void bind(InetAddr* ipaddr);
        void listen();
        int accept(InetAddr* ipaddr);
        int getFd();
    };
}

#endif