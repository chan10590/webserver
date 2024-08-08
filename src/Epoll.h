#ifndef WEB_EPOLL_H_
#define WEB_EPOLL_H_
#include <sys/epoll.h>
#include <vector>
#include "../util/noncopyable.h"
#include "Channel.h"

namespace web{
class Noncopyable;
class Channel;
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

    std::vector<Channel*> triggerNodeSet(int timeout = -1);
    void addChannel(Channel* channel);
};    
}

#endif