#ifndef SRC_INC_SERVER_H_
#define SRC_INC_SERVER_H_

#include <arpa/inet.h>
#include <map>
#include <vector>
#include "noncopyable.h"

namespace web {
class EventLoop;
class Listener;
class Connection;
class Socket;
class ThreadPool;
/*绑定套接字，设置回调*/
class Server : public Noncopyable {
 private:
    EventLoop* m_mainloop;
    Listener* m_listener;
    std::map<int, Connection*> m_connections;
    uint16_t m_port;
    uint32_t m_event;
    std::vector<EventLoop*> m_subloops;
    ThreadPool* m_thpool;
    int m_next_subloop_index;  // 轮询索引，保证负载均衡，
 public:
    Server(EventLoop* mainloop, uint16_t port, uint32_t ev);
    ~Server();

    void newConnection(Socket* sock);
    void deleteConnection(Socket* sock);
};
}  // namespace web

#endif  // SRC_INC_SERVER_H_
