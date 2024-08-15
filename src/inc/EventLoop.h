#ifndef SRC_INC_EVENTLOOP_H_
#define SRC_INC_EVENTLOOP_H_

#include <functional>
#include "Channel.h"
#include "Epoll.h"

namespace web {
class Epoll;
class Channel;
class ThreadPool;
/*循环处理触发的事件集合*/
class EventLoop {
 private:
    Epoll* m_ep;
    ThreadPool* m_threadpool;
    bool m_quit;

 public:
    EventLoop();
    ~EventLoop();

    void loop();
    void addChannel(Channel* ch);
    void addTask(std::function<void()> task);
};
}  // namespace web

#endif  // SRC_INC_EVENTLOOP_H_
