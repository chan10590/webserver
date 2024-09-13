#include "Channel.h"
#include <utility>
#include "EventLoop.h"
#include "Logger.h"

// 构造Channel对象
Channel::Channel(EventLoop* evloop, int fd) : _evloop(evloop), _fd(fd), _event(0), _ready(0), _in_epoll(false) {}
Channel::~Channel() {
    LOG_INFO("Channel deconstructor");
    if (_in_epoll) {
        delChannel();
        close(_fd);
    }
}

// 执行事件对应回调
void Channel::handleEvent() const {
    if (_ready & (EPOLLIN)) {
        if (_rdevent_cb) {
            LOG_INFO("Handling event with callback");
            _rdevent_cb();
        }
    }
    if (_ready & (EPOLLOUT)) {
        LOG_INFO("EPOLLOUT event occurred.");
        _wrevent_cb();
    }
    if (_ready & EPOLLERR) {
        LOG_ERROR("EPOLLERR event occurred.");
    }
    if (_ready & EPOLLHUP) {
        LOG_ERROR("EPOLLHUP event occurred.");
    }
}
// 设置读回调
void Channel::setRDCallback(const std::function<void()>& cb) { _rdevent_cb = std::move(cb); }
// 设置写回调
void Channel::setWRCallback(const std::function<void()>& cb) { _wrevent_cb = std::move(cb); }

// 监听读事件或写事件，添加到epoll
void Channel::setAddChannel(uint32_t ev) {
    _event |= ev;
    _evloop->addChannel(this);
}
// 从epoll删除
void Channel::delChannel() {
    if (_in_epoll) {
        _evloop->delChannel(this);
        _in_epoll = false;
    }
}
// 修改epoll上的节点为边沿触发
void Channel::useET() {
    _event |= EPOLLET;
    _evloop->addChannel(this);
}

// 返回文件描述符
int Channel::getFd() const { return _fd; }
// 返回监听事件
uint32_t Channel::getEvent() const { return _event; }
// 返回就绪事件
uint32_t Channel::getReady() const { return _ready; }
// 设置就绪事件
void Channel::setReady(uint32_t ready) { _ready = ready; }
// 返回Channel是否在epoll上的标志
bool Channel::getInEpoll() const { return _in_epoll; }
// 设置Channel是否在epoll上的标志
void Channel::setInEpoll(bool inepoll) { _in_epoll = inepoll; }
