#ifndef WEB_EPOLL_H_
#define WEB_EPOLL_H_
#include <sys/epoll.h>
#include <vector>
#include "../util/noncopyable.h"

namespace web{
class Noncopyable;
/* epoll相关操作
 * epfd epoll根节点
 * evmax 结构体数组，每个元素为epoll_event，包含fd及相应监听事件
 */
class Epoll:public Noncopyable
{
private:
    int m_epfd;
    struct epoll_event* m_evarr;
public:
    Epoll();
    ~Epoll();
    void addNode(int fd, uint32_t ev_events);
    //void delNode();
    std::vector<epoll_event*> triggerNodeSet(int timeout = -1);
};    
}

#endif