#include "EventLoop.h"
#include <iostream>

namespace web {
    void EventLoop::loop() {
        while(!m_quit) {
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