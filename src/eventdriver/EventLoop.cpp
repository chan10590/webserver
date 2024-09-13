#include "EventLoop.h"
#include "Channel.h"
#include "Logger.h"

EventLoop::EventLoop() : _ep(std::make_unique<Epoll>()) {}

EventLoop::~EventLoop() {}

void EventLoop::loop() const {
    while (true) {
        for (Channel* ch : _ep->trigChannelSet()) {
            LOG_INFO("handle event");
            ch->handleEvent();
        }
    }
}

void EventLoop::addChannel(Channel* ch) const { _ep->addChannel(ch); }

void EventLoop::delChannel(Channel* ch) const { _ep->deleteChannel(ch); }
