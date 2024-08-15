#include "Channel.h"
#include <iostream>

namespace web {
Channel::Channel(EventLoop* evloop, int fd)
    : m_evloop(evloop), m_fd(fd), m_events(0), m_ready(0), m_in_epoll(false) {}

Channel::~Channel() {}

int Channel::getFd() { return m_fd; }

uint32_t Channel::getEvents() { return m_events; }

uint32_t Channel::getReady() { return m_ready; }

void Channel::setReady(uint32_t ready) { m_ready = ready; }

bool Channel::getInEpoll() { return m_in_epoll; }

void Channel::setInEpoll(bool inepoll) { m_in_epoll = inepoll; }

void Channel::setRDCallback(std::function<void()> cb) { m_rdevent_cb = cb; }

void Channel::setWRCallback(std::function<void()> cb) { m_wrevent_cb = cb; }

/*设置监听事件为读事件，边沿触发，同时上epoll树*/
void Channel::setAddChannel(uint32_t ev) {
    m_events |= ev;
    m_evloop->addChannel(this);
}

void Channel::useET() {
    m_events |= EPOLLET;
    m_evloop->addChannel(this);
}

void Channel::handleEvent() {
    if (m_ready & (EPOLLIN | EPOLLPRI)) {
        m_rdevent_cb();
    }
    if (m_ready & (EPOLLOUT)) {
        m_wrevent_cb();
    }
}

}  // namespace web
