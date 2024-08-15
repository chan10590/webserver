#include "Connection.h"
#include <string.h>
#include <unistd.h>
#include <iostream>
#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"

namespace web {
/*为新建套接字设置回调，回调时处理通信事件*/
Connection::Connection(EventLoop* evloop, Socket* sock, uint32_t ev)
    : m_evloop(evloop), m_sock(sock), m_event(ev), m_connect_chhanel(nullptr) {
    m_connect_chhanel = new Channel(m_evloop, sock->getFd());
    std::function<void()> commun_cb = std::bind(&Connection::handleCommun, this);
    m_connect_chhanel->setRDCallback(commun_cb);
    m_connect_chhanel->setAddChannel(m_event);
    m_connect_chhanel->useET();
}

Connection::~Connection() {
    delete m_connect_chhanel;
    delete m_sock;
}

void Connection::handleCommun() {
    char* msg = const_cast<char*>("hello\n");
    char buf[1024];
    int fd = m_sock->getFd();
    while (1) {
        memset(buf, 0, sizeof(buf));
        int ret = read(fd, buf, sizeof(buf));
        // 读取到通信请求并响应
        if (ret > 0) {
            std::cout << "client " << fd << " send: " << buf << std::endl;
            write(fd, msg, strlen(msg));
        } else if (ret == 0) {
            // 客户端断开连接，需要从epoll上移除相应的套接字及事件
            std::cout << "client " << fd << " break connection!" << std::endl;
            m_breakconc_cb(m_sock);
            // web::errorExit(epoll_ctl(epfd,EPOLL_CTL_DEL,evi_fd,&trig_evs[i])==-1,"epoll
            // delete communication socket fail!");
            break;
        } else if (ret == -1 && errno == EINTR) {
            // 正常中断，再次尝试读取
            std::cout << "normal interpret! read again!" << std::endl;
            continue;
        } else if (ret == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
            // 对于非阻塞IO，已经读完数据，没有数据可读取
            std::cout << "read all datas! no readable data!" << std::endl;
            break;
        } else {
            std::cout << "connect reset!" << std::endl;
            m_breakconc_cb(m_sock);
            break;
        }
    }
}

/*为断开连接时设立回调*/
void Connection::setBreakConnectionCb(std::function<void(Socket*)> cb) { m_breakconc_cb = cb; }
}  // namespace web
