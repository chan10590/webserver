#ifndef EPOLL_H_
#define EPOLL_H_

#include <sys/epoll.h>
#include <unistd.h>
#include <mutex>
#include <vector>
#include "noncopyable.h"

class Channel;
class Epoll : public Noncopyable {
 public:
    Epoll();
    ~Epoll();

    void addChannel(Channel* ch) const;
    void deleteChannel(Channel* ch) const;

    std::vector<Channel*> trigChannelSet(int timeout = -1) const;

 private:
    int _epfd;
    struct epoll_event* _evarr;
    mutable std::mutex _mutex;
};

#endif  // END EPOLL_H_
