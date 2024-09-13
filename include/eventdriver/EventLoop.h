#ifndef EVENTLOOP_H_
#define EVENTLOOP_H_

#include <functional>
#include <memory>
#include "Epoll.h"

class Channel;
class EventLoop : public Noncopyable {
 public:
    EventLoop();
    ~EventLoop();

    void loop() const;
    void addChannel(Channel* ch) const;
    void delChannel(Channel* ch) const;

 private:
    std::unique_ptr<Epoll> _ep;
};

#endif  // END EVENTLOOP_H_
