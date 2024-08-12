#include "Listener.h"
#include <functional>
#include <iostream>

namespace web {
    /*创建监听套接字，并为监听套接字设置回调，回调是添加一个新的通信套接字*/
    Listener::Listener(EventLoop* evloop, uint16_t port, uint32_t ev) : m_evloop(evloop), m_port(port), m_event(ev) {
        m_listen_sock = new Socket;
        m_listen_sock->setsockopt();
        m_listen_addr = new InetAddr("127.0.0.1", m_port);
        m_listen_sock->bind(m_listen_addr);
        m_listen_sock->listen();
        m_listen_sock->setnonblocking();// 非阻塞

        m_listen_channel = new Channel(m_evloop, m_listen_sock->getFd());
        std::function<void()> newconc_cb = std::bind(&Listener::addNewSocket, this);
        m_listen_channel->setCallback(newconc_cb);
        m_listen_channel->setAddChannel(m_event);
        delete m_listen_addr;
    }

    Listener::~Listener() {
        delete m_listen_sock;
        delete m_listen_channel;
    }

    void Listener::addNewSocket() {
        InetAddr* clnt_addr = new InetAddr;
        int sfd = m_listen_sock->accept(clnt_addr);
        Socket* conc_sock = new Socket(sfd);//内存泄漏，智能指针管理
        conc_sock->setnonblocking();// 非阻塞
        std::cout << "client " << sfd << " connect!" << std::endl;
        m_commun_cb(conc_sock);
        delete clnt_addr;
    }

    /*为新建的通信套接字设置回调*/
    void Listener::setCommunCb(std::function<void(Socket* )> cb) {
        m_commun_cb = cb;
    }
}