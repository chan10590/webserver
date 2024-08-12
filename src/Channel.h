#ifndef WEB_CHANNEL_H_
#define WEB_CHANNEL_H_

#include "../util/noncopyable.h"
#include "EventLoop.h"
#include <functional>

namespace web {
    class EventLoop;
    /*自定义epoll事件节点，可通过回调处理事件*/
    class Channel : public Noncopyable {
    private:
        EventLoop* m_evloop;
        int m_fd;
        uint32_t m_events;//文件描述符要监听的事件（读/写，边沿触发/水平触发）
        uint32_t m_revents;//具体事件，如（是http报文读事件，还是文件传输请求读事件），将不同业务分离
        bool m_in_epoll;
        std::function<void()> m_event_cb;//事件回调函数，发生对应事件时的处理函数
    public:
        Channel(EventLoop* evloop, int fd) : m_evloop(evloop), m_fd(fd), m_events(0), m_revents(0), m_in_epoll(false) {}
        ~Channel() {}

        int getFd();
        uint32_t getEvents();
        uint32_t getRevents();
        void setRevents(uint32_t rev);
        bool getInEpoll();
        void setInEpoll();
        void setCallback(std::function<void()> cb);

        void setAddChannel (uint32_t ev);
        void handleEvent();
    };
}

#endif
