#include "EventLoop.h"
#include <iostream>

namespace web {
    EventLoop::EventLoop() : m_ep(new Epoll), m_quit(false) {
    }

    EventLoop::~EventLoop() {
        delete m_ep;
    }

    void EventLoop::loop() {
        while(!m_quit) {
            std::cout << "------------------------------------" << std::endl;
            std::cout << "Waiting for events..." << std::endl;
            std::vector<Channel* > trigchs;
            trigchs = m_ep->trigChannelSet();
            for(auto it = trigchs.begin(); it != trigchs.end(); ++it) {
                (*it)->handleEvent();
                std::cout << "------------------------------------" << std::endl;
            }
        }
    }
    
    void EventLoop::addChannel(Channel* ch) {
        m_ep->addChannel(ch);
    }
}