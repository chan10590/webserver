#include "Listener.h"
#include <fcntl.h>
#include "EventLoop.h"
#include "Logger.h"

// 创建监听套接字，并为监听套接字设置回调，epoll监听事件
Listener::Listener(EventLoop* evloop, uint16_t port, uint32_t ev) : _evloop(evloop), _port(port), _event(ev) {
    _listen_sock = std::make_unique<Socket>();
    _listen_sock->create();
    _listen_sock->setsockopt();
    _listen_sock->bind("127.0.0.1", _port);
    _listen_sock->listen();
    LOG_INFO("Create a listen socket %d for main reactor!", _listen_sock->getFd());
    // m_listen_sock->setnonblocking();  // 监听套接字阻塞更好

    LOG_INFO("Create a listen socket Channel");
    _listen_channel = std::make_unique<Channel>(_evloop, _listen_sock->getFd());
    LOG_INFO("Set RDcallback for listen socket Channel");
    _listen_channel->setRDCallback([this]() {
        LOG_INFO("to addNewSocket");
        this->addNewSocket();
    });
    LOG_INFO("Add listen socket Channel to epoll");
    _listen_channel->setAddChannel(_event);
}
Listener::~Listener() { LOG_INFO("Listener deconstructor"); }

// 建立连接，新建通信套接字
void Listener::addNewSocket() const {
    int clnt_fd = -1;
    _listen_sock->accept(clnt_fd);
    LOG_INFO("addNewSocket Function --- create a commun socket %d", clnt_fd);
    fcntl(clnt_fd, F_SETFL, fcntl(clnt_fd, F_GETFL) | O_NONBLOCK);  // 新接受到的连接设置为非阻塞式
    if (_commun_cb) _commun_cb(clnt_fd);
}

// 设置回调
void Listener::setCommunCb(const std::function<void(int)>& cb) { _commun_cb = cb; }
