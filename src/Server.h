#ifndef WEB_SERVER_H_
#define WEB_SERVER_H_

#include <arpa/inet.h>

namespace web {
    class EventLoop;
    class Socket;
    /*绑定套接字，设置回调*/
    class Server {
    private:
        EventLoop* m_evloop;
        uint16_t m_port;
        uint32_t m_event;
    public:
        Server(EventLoop* evloop, uint16_t port, uint32_t ev);
        ~Server() {}

        void newConnection(Socket* sock);
        void handleReadCommun(int fd);
    };
}

#endif