#ifndef SRC_INC_EPOLL_H_
#define SRC_INC_EPOLL_H_

#include <sys/epoll.h>
#include <vector>
#include "noncopyable.h"

#define MAXEVENT 1024

namespace web {
class Noncopyable;
class Channel;
/* epoll相关操作，对节点进行操作
 * epfd epoll根节点
 * evmax 结构体数组，每个元素为epoll_event，包含fd及相应监听事件
 */
class Epoll : public Noncopyable {
 private:
    int m_epfd;
    struct epoll_event* m_evarr;

 public:
    Epoll();
    ~Epoll();

    std::vector<Channel*> trigChannelSet(int timeout = -1);
    void addChannel(Channel* channel);
    void deleteChannel(Channel* channel);
};
}  // namespace web

#endif  // SRC_INC_EPOLL_H_
