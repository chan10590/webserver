#include "Connection.h"
#include <unistd.h>
#include <cassert>
#include <cstring>
#include "EventLoop.h"
#include "Logger.h"

const size_t BUFFER_SIZE = 1024;

// epoll监听通信套接字事件
Connection::Connection(EventLoop* evloop, int fd, uint32_t ev) : _event(ev), _state(State::Connected) {  //
    _sock = std::make_unique<Socket>();
    _sock->setFd(fd);
    _connect_channel = std::make_unique<Channel>(evloop, fd);
    LOG_INFO("add communication socket Channel to sub epoll!");
    _connect_channel->setAddChannel(ev);  //
    _connect_channel->useET();
    _rdbuf = std::make_unique<Buffer>();
    _wrbuf = std::make_unique<Buffer>();
}
Connection::~Connection() {  //
    LOG_INFO("Connection deconstructor");
    if (_state == State::Connected) {
        Close();
    }
}

void Connection::setBreakConnectionCb(const std::function<void(int)>& cb) { _breakconc_cb = cb; }
void Connection::SetOnConnectCallback(const std::function<void(Connection*)>& cb) {
    _onconc_cb = cb;
    LOG_INFO("SetOnConnectCallback Function --- set RDcallback for commun sock, cb is Server::_onconc_cb");
    _connect_channel->setRDCallback([this]() {
        LOG_INFO("Read request");
        this->Read();
        if (_onconc_cb) {
            LOG_INFO("to handleConnection");
            _onconc_cb(this);  // 检查 _onconc_cb 是否有效
        } else {
            LOG_ERROR("OnConnectCallback is not set or is empty");  // 添加错误日志以帮助调试
        }
    });
}
void Connection::Send(std::string msg) {
    setSendBuffer(std::move(msg));
    Write();
}
void Connection::Read() {
    if (_state != State::Connected) {
        LOG_ERROR("Connection is not connected, cannot read.");
        return;
    }
    _rdbuf->clear();
    int fd = _sock->getFd();
    char buf[BUFFER_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read(fd, buf, sizeof(buf))) > 0) {
        _rdbuf->append(buf, bytes_read);
    }
    while (true) {
        bytes_read = read(fd, buf, sizeof(buf));
        if (bytes_read > 0) {
            _rdbuf->append(buf, bytes_read);
        } else if (bytes_read == 0) {
            LOG_INFO("Client closed connection");
            _state = State::Closed;
            Close();
            break;
        } else if (bytes_read == -1) {
            if (errno == EINTR) {
                continue;
            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            } else {
                LOG_ERROR("Read error: %s", strerror(errno));
                _state = State::Closed;
                Close();
                break;
            }
        }
    }
}
void Connection::Write() {
    if (_state != State::Connected) {
        LOG_ERROR("Connection is not connected, cannot write.");
        return;
    }
    int fd = _sock->getFd();
    const std::string& data = _wrbuf->toStr();
    size_t data_left = data.size();
    const char* buf = data.data();

    while (data_left > 0) {
        ssize_t bytes_write = write(fd, buf + data.size() - data_left, data_left);
        if (bytes_write >= 0) {
            data_left -= bytes_write;
        } else if (bytes_write == -1) {
            if (errno == EINTR) {
                continue;
            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            } else {
                LOG_ERROR("Write error: %s", strerror(errno));
                _state = State::Closed;
                Close();
                break;
            }
        }
    }
    if (data_left == 0) {
        _wrbuf->clear();
    }
}
void Connection::Close() {
    if (_breakconc_cb) {
        _breakconc_cb(_sock->getFd());
    }
    LOG_INFO("Connection closed");
}

Connection::State Connection::getState() const { return _state; }
void Connection::setSendBuffer(const std::string& str) { _wrbuf->setBuf(str); }
Buffer* Connection::getReadBuffer() { return _rdbuf.get(); }
std::string Connection::readBuffer() const { return _rdbuf->toStr(); }
Buffer* Connection::getSendBuffer() { return _wrbuf.get(); }
std::string Connection::sendBuffer() const { return _wrbuf->toStr(); }
Socket* Connection::getSocket() { return _sock.get(); }
Channel* Connection::getChannel() { return _connect_channel.get(); }
