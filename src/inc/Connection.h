#ifndef SRC_INC_CONNECTION_H_
#define SRC_INC_CONNECTION_H_

#include <cstdint>
#include <functional>
#include "noncopyable.h"

namespace web {
class Noncopyable;
class EventLoop;
class Channel;
class Socket;
/*利用建立的连接进行通信*/
class Connection : public Noncopyable {
 private:
    EventLoop* m_evloop;
    Socket* m_sock;
    uint32_t m_event;
    Channel* m_connect_chhanel;
    std::function<void(Socket*)> m_breakconc_cb;

 public:
    Connection(EventLoop* evloop, Socket* sock, uint32_t ev);
    ~Connection();

    void handleCommun();
    void setBreakConnectionCb(std::function<void(Socket*)> cb);
};
}  // namespace web

#endif  // SRC_INC_CONNECTION_H_
