#ifndef SRC_INC_CHANNEL_H_
#define SRC_INC_CHANNEL_H_

#include <cstdint>
#include <functional>
#include "EventLoop.h"
#include "noncopyable.h"

namespace web {
class EventLoop;
/*自定义epoll事件节点，可通过回调处理事件*/
class Channel : public Noncopyable {
 private:
    EventLoop* m_evloop;
    int m_fd;
    uint32_t m_events;  // 文件描述符要监听的事件（读/写，边沿触发/水平触发）
    uint32_t m_ready;   // 发生的事件
    bool m_in_epoll;
    std::function<void()> m_rdevent_cb;  // 读事件回调函数，发生对应事件时的处理函数
    std::function<void()> m_wrevent_cb;  // 读事件回调函数，发生对应事件时的处理函数

 public:
    Channel(EventLoop* evloop, int fd);
    ~Channel();

    int getFd();
    uint32_t getEvents();
    uint32_t getReady();
    void setReady(uint32_t ready);
    bool getInEpoll();
    void setInEpoll(bool inepoll = true);
    void setRDCallback(std::function<void()> cb);
    void setWRCallback(std::function<void()> cb);

    void setAddChannel(uint32_t ev);
    void useET();
    void handleEvent();
};
}  // namespace web

#endif  // SRC_INC_CHANNEL_H_
