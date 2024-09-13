#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <functional>
#include <memory>
#include <string>
#include "Buffer.h"
#include "Channel.h"
#include "Socket.h"
#include "noncopyable.h"

class EventLoop;
class Connection : public Noncopyable {
 public:
    enum class State { Connected, Closed };

    Connection(EventLoop* evloop, int fd, uint32_t ev);
    ~Connection();

    void setBreakConnectionCb(const std::function<void(int)>& cb);
    void SetOnConnectCallback(const std::function<void(Connection*)>& cb);
    void Send(std::string msg);
    void Close();

    void Read();
    void Write();

    State getState() const;
    void setSendBuffer(const std::string& str);
    Buffer* getReadBuffer();
    std::string readBuffer() const;
    Buffer* getSendBuffer();
    std::string sendBuffer() const;
    Socket* getSocket();
    Channel* getChannel();

 private:
    std::unique_ptr<Socket> _sock;
    uint32_t _event;
    std::unique_ptr<Channel> _connect_channel;
    State _state;
    std::unique_ptr<Buffer> _rdbuf;
    std::unique_ptr<Buffer> _wrbuf;
    std::function<void(int)> _breakconc_cb;
    std::function<void(Connection*)> _onconc_cb;
};

#endif  // END CONNECTION_H_
