#ifndef WEB_EVENTLOOP_H_
#define WEB_EVENTLOOP_H_

#include "Epoll.h"
#include "Channel.h"

namespace web {
    class Epoll;
    class Channel;
    /*循环处理触发的事件集合*/
    class EventLoop {
    private:
        Epoll* m_ep;
        bool m_quit;
    public:
        EventLoop();
        ~EventLoop();

        void loop();
        void addChannel(Channel* ch);
    };
}

#endif