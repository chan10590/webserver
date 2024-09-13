#ifndef LISTENER_H_
#define LISTENER_H_

#include <cstdint>
#include <functional>
#include <memory>
#include "Channel.h"
#include "Socket.h"
#include "noncopyable.h"

class EventLoop;
class Listener : public Noncopyable {
 public:
    Listener(EventLoop* evloop, uint16_t port, uint32_t ev);
    ~Listener();

    void setCommunCb(const std::function<void(int)>& cb);

 private:
    void addNewSocket() const;

    EventLoop* _evloop;
    uint16_t _port;
    uint32_t _event;
    std::unique_ptr<Socket> _listen_sock;
    std::unique_ptr<Channel> _listen_channel;
    std::function<void(int)> _commun_cb;
};

#endif  // END LISTENER_H_
