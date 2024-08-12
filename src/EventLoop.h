#ifndef WEB_EVENTLOOP_H_
#define WEB_EVENTLOOP_H_

#include "Epoll.h"

namespace web {
    class Epoll;
    class Channel;
    /*循环处理触发的事件集合*/
    class EventLoop {
    private:
        Epoll* m_ep;
        bool m_quit;
    public:
        EventLoop() : m_ep(new Epoll()), m_quit(false) {
        }
        ~EventLoop() {
            if(m_ep) {
                delete m_ep;
                m_ep = nullptr;
            }
        }

        void loop();
        void addChannel(Channel* ch);
    };
}

#endif