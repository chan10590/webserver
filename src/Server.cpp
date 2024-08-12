#include "../util/util.h"
#include "Server.h"
#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Listener.h"
#include "Connection.h"
#include "Socket.h"
#include <iostream>
#include <functional>
#include <unistd.h>

namespace web {
    Server::Server(EventLoop* evloop, uint16_t port, uint32_t ev) : m_evloop(evloop), m_port(port), m_event(ev) {
        m_listener = new Listener(m_evloop, m_port, m_event);
        std::function<void(Socket* )> cb = std::bind(&Server::newConnection, this, std::placeholders::_1);
        m_listener->setCommunCb(cb);
    }

    Server::~Server() {
        delete m_listener;
    }

    void Server::newConnection(Socket* sock) {
        std::cout << "bind communsock cb" << std::endl;
        Connection* conc = new Connection(m_evloop, sock, m_event);
        std::function<void(Socket* )> cb = std::bind(&Server::deleteConnection, this, std::placeholders::_1);
        conc->setBreakConnectionCb(cb);
        m_connections[sock->getFd()] = conc; 
    }

    void Server::deleteConnection(Socket* sock) {
        Connection* conc = m_connections[sock->getFd()];
        m_connections.erase(sock->getFd());
        delete conc;
    }
}