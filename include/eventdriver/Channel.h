#ifndef CHANNEL_H_
#define CHANNEL_H_

#include <cstdint>
#include <functional>
#include "noncopyable.h"

class EventLoop;
class Channel : public Noncopyable {
 public:
    Channel(EventLoop* evloop, int fd);
    ~Channel();

    void handleEvent() const;
    void setRDCallback(const std::function<void()>& cb);
    void setWRCallback(const std::function<void()>& cb);

    void setAddChannel(uint32_t ev);
    void useET();
    void delChannel();

    int getFd() const;
    uint32_t getEvent() const;
    uint32_t getReady() const;
    void setReady(uint32_t ready);
    bool getInEpoll() const;
    void setInEpoll(bool inepoll);

 private:
    EventLoop* _evloop;
    int _fd;
    uint32_t _event;
    uint32_t _ready;
    bool _in_epoll;
    std::function<void()> _rdevent_cb;
    std::function<void()> _wrevent_cb;
};

#endif  // END CHANNEL_H_
