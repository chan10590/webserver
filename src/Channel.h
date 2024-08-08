#ifndef WEB_CHANNEL_H_
#define WEB_CHANNEL_H_
#include "../util/noncopyable.h"
#include "Epoll.h"

namespace web{
class Epoll;
/*自定义epoll事件节点*/
class Channel:public Noncopyable{
private:
    Epoll* m_ep;
    int m_fd;
    uint32_t m_events;//文件描述符要监听的事件（读/写，边沿触发/水平触发）
    uint32_t m_revents;//具体事件，如（是http报文读事件，还是文件传输请求读事件），将不同业务分离
    bool m_in_epoll;
public:
    Channel(Epoll* ep, int fd):m_ep(ep), m_fd(fd), m_events(0), m_revents(0), m_in_epoll(false){}
    ~Channel(){}

    int getFd();
    uint32_t getEvents();
    uint32_t getRevents();
    void setRevents(uint32_t rev);
    bool getInEpoll();
    void setInEpoll();

    void setAddChannel (uint32_t ev);
};
}

#endif