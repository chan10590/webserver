#ifndef SRC_INC_LISTENER_H_
#define SRC_INC_LISTENER_H_

#include "Channel.h"
#include "EventLoop.h"
#include "InetAddr.h"
#include "Socket.h"
#include "noncopyable.h"

namespace web {
class Noncopyable;
class EventLoop;
class Socket;
class InetAddr;
class Channel;
/*用于监听新连接*/
class Listener : public Noncopyable {
 private:
    EventLoop* m_evloop;
    uint16_t m_port;
    uint32_t m_event;
    Socket* m_listen_sock;
    InetAddr* m_listen_addr;
    Channel* m_listen_channel;
    std::function<void(Socket*)> m_commun_cb;

 public:
    Listener(EventLoop* evloop, uint16_t port, uint32_t ev);
    ~Listener();

    void addNewSocket();
    void setCommunCb(std::function<void(Socket*)> cb);
};
}  // namespace web

#endif  // SRC_INC_LISTENER_H_
