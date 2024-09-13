#ifndef SERVER_H_
#define SERVER_H_

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>
#include "Connection.h"
#include "EventLoop.h"
#include "Listener.h"
#include "Socket.h"
#include "ThreadPool.h"
#include "noncopyable.h"

class EventLoop;
class Listener;
class Connection;
class Socket;
class ThreadPool;
class Server : public Noncopyable {
 public:
    Server(uint16_t port, uint32_t ev);
    ~Server();

    void Start();
    void onConnection(std::function<void(Connection*)> cb);

 private:
    void newConnection(int fd);
    void deleteConnection(int fd);

    std::unique_ptr<EventLoop> _mainloop;
    std::unique_ptr<Listener> _listener;
    uint16_t _port;
    uint32_t _event;
    int _next_subloop_index;  // 轮询索引，保证负载均衡
    std::unique_ptr<ThreadPool> _thpool;
    std::vector<std::unique_ptr<EventLoop>> _subloops;
    std::unordered_map<int, std::unique_ptr<Connection>> _connections;
    std::function<void(Connection*)> _onconc_cb;
    std::mutex _conn_mtx;
};

#endif  // END SERVER_H_
