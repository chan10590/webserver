#ifndef WEB_SERVER_H_
#define WEB_SERVER_H_

#include "../util/noncopyable.h"
#include <arpa/inet.h>
#include <map>

namespace web {
    class EventLoop;
    class Listener;
    class Connection;
    class Socket;
    /*绑定套接字，设置回调*/
    class Server : public Noncopyable {
    private:
        EventLoop* m_evloop;
        Listener* m_listener;
        std::map<int, Connection* > m_connections;
        uint16_t m_port;
        uint32_t m_event;
    public:
        Server(EventLoop* evloop, uint16_t port, uint32_t ev);
        ~Server();

        void newConnection(Socket* sock);
        void deleteConnection(Socket* sock);
    };
}

#endif