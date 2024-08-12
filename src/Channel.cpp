#include "Channel.h"

namespace web {
    int Channel::getFd() {
        return m_fd;
    }

    uint32_t Channel::getEvents() {
        return m_events;
    }

    uint32_t Channel::getRevents() {
        return m_revents;
    }

    void Channel::setRevents(uint32_t rev) {
        m_revents = rev;
    }

    bool Channel::getInEpoll() {
        return m_in_epoll;
    }

    void Channel::setInEpoll() {
        m_in_epoll = true;
    }

    void Channel::setCallback(std::function<void()> cb) {
        m_event_cb = cb;
    }

    /*设置监听事件为读事件，边沿触发，同时上epoll树*/
    void Channel::setAddChannel(uint32_t ev) {
        m_events = ev;
        m_evloop->addChannel(this);
    }

    void Channel::handleEvent() {
        m_event_cb();
    }
    
}