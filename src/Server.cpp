#include "Server.h"
#include <unistd.h>
#include <functional>
#include <iostream>
#include "Channel.h"
#include "Connection.h"
#include "EventLoop.h"
#include "Listener.h"
#include "Socket.h"
#include "ThreadPool.h"

namespace web {
Server::Server(EventLoop* mainloop, uint16_t port, uint32_t ev)
    : m_mainloop(mainloop),
      m_listener(nullptr),
      m_port(port),
      m_event(ev),
      m_next_subloop_index(0) {
    m_listener = new Listener(m_mainloop, m_port, m_event);
    std::function<void(Socket*)> cb =
        std::bind(&Server::newConnection, this, std::placeholders::_1);
    m_listener->setCommunCb(cb);

    int pool_size = std::thread::hardware_concurrency();
    std::cout << "server thread num: " << pool_size << std::endl;
    m_thpool = new ThreadPool(pool_size);
    for (int i = 0; i < pool_size; ++i) {
        m_subloops.push_back(new EventLoop);
    }

    for (int i = 0; i < pool_size; ++i) {
        std::function<void()> subloop = std::bind(&EventLoop::loop, m_subloops[i]);
        m_thpool->addTask2Queue(subloop);
    }
}

Server::~Server() {
    delete m_listener;
    delete m_thpool;
    for (EventLoop* subloop : m_subloops) {
        delete subloop;
    }
    m_subloops.clear();
}

void Server::newConnection(Socket* sock) {
    // 使用轮询法选择子反应堆
    int index = m_next_subloop_index;
    m_next_subloop_index = (m_next_subloop_index + 1) % m_subloops.size();

    // 创建新连接，并将其分配到选定的子反应堆
    Connection* conc = new Connection(m_subloops[index], sock, m_event);
    std::function<void(Socket*)> cb =
        std::bind(&Server::deleteConnection, this, std::placeholders::_1);
    conc->setBreakConnectionCb(cb);
    m_connections[sock->getFd()] = conc;
}

void Server::deleteConnection(Socket* sock) {
    auto it = m_connections.find(sock->getFd());
    if (it != m_connections.end()) {
        Connection* conc = m_connections[sock->getFd()];
        m_connections.erase(sock->getFd());
        delete conc;
    }
}
}  // namespace web
