#include "Server.h"
#include <unistd.h>
#include <cassert>
#include "Channel.h"
#include "Logger.h"

Server::Server(uint16_t port, uint32_t ev) : _port(port), _event(ev), _next_subloop_index(0) {
    LOG_INFO("Create mainreactor");
    _mainloop = std::make_unique<EventLoop>();
    _listener = std::make_unique<Listener>(_mainloop.get(), _port, _event);
    LOG_INFO("set newConnection callback");
    _listener->setCommunCb([this](int fd) {
        LOG_INFO("to newConnection");
        this->newConnection(fd);
    });

    int pool_size = static_cast<int>(std::thread::hardware_concurrency());
    _thpool = std::make_unique<ThreadPool>(pool_size);
    LOG_INFO("create a thread pool with %d threads", pool_size);

    for (int i = 0; i < pool_size; ++i) {
        LOG_INFO("create a subreactor");
        _subloops.emplace_back(std::make_unique<EventLoop>());
    }
}
Server::~Server() {}

void Server::Start() {
    LOG_INFO("subloop to task queue");
    for (const auto& subloop : _subloops) {
        _thpool->addTask([subloop = subloop.get()]() { subloop->loop(); });
    }
    LOG_INFO("main loop");
    _mainloop->loop();
}

void Server::onConnection(std::function<void(Connection*)> cb) {
    LOG_INFO("onConnection Function --- Server::_onconc_cb");
    _onconc_cb = std::move(cb);
}

void Server::newConnection(int fd) {
    LOG_INFO("newConnection Function --- create and dispatch connection");
    // 使用轮询法选择子反应堆
    _next_subloop_index = (_next_subloop_index + 1) % _subloops.size();
    EventLoop* selected_subloop = _subloops[_next_subloop_index].get();

    std::unique_ptr<Connection> conn = std::make_unique<Connection>(selected_subloop, fd, _event);
    LOG_INFO("dispatch connection to subreactor %d", _next_subloop_index);
    LOG_INFO("set OnConnect callback and BreakConnection");
    conn->SetOnConnectCallback(_onconc_cb);
    conn->setBreakConnectionCb([this](int fd) {
        LOG_INFO("to deleteConnection");
        this->deleteConnection(fd);
    });
    {
        std::lock_guard<std::mutex> lock(_conn_mtx);
        _connections[fd] = std::move(conn);
    }
    LOG_INFO("insert fd %d-connection pair!", fd);
}
void Server::deleteConnection(int fd) {
    LOG_INFO("deleteConnection Function --- erase fd-connection pair");
    std::lock_guard<std::mutex> lock(_conn_mtx);
    auto it = _connections.find(fd);
    if (it != _connections.end()) {
        _connections.erase(it);
        LOG_INFO("Connection with fd %d deleted from map", fd);
    }
}
