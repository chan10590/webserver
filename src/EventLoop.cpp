#include "EventLoop.h"
#include <iostream>
#include "ThreadPool.h"

namespace web {
EventLoop::EventLoop() : m_ep(new Epoll), m_threadpool(new ThreadPool), m_quit(false) {}

EventLoop::~EventLoop() {
    delete m_ep;
    delete m_threadpool;
}

void EventLoop::loop() {
    while (!m_quit) {
        std::cout << "------------------------------------" << std::endl;
        std::cout << "Waiting for events..." << std::endl;
        std::vector<Channel*> trigchs;
        trigchs = m_ep->trigChannelSet();
        for (auto it = trigchs.begin(); it != trigchs.end(); ++it) {
            (*it)->handleEvent();
            std::cout << "------------------------------------" << std::endl;
        }
    }
}

void EventLoop::addChannel(Channel* ch) { m_ep->addChannel(ch); }

void EventLoop::addTask(std::function<void()> task) { m_threadpool->addTask2Queue(task); }
}  // namespace web
