#include "Epoll.h"
#include <cstring>
#include "Channel.h"
#include "Logger.h"
#include "util.h"

#define MAX_EVENTS 1024

// 构造epoll对象
Epoll::Epoll() : _epfd(epoll_create1(0)), _evarr(new epoll_event[MAX_EVENTS]) {
    LOG_INFO("Create epoll(epfd: %d)", _epfd);
    errorExit(_epfd == -1, "epoll create fail!");
    memset(_evarr, 0, sizeof(*_evarr) * MAX_EVENTS);
}
Epoll::~Epoll() {
    if (_epfd != -1) {
        close(_epfd);
    }
    delete[] _evarr;
}

// 添加Channel到epoll
void Epoll::addChannel(Channel* ch) const {
    int fd = ch->getFd();
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data.ptr = ch;
    ev.events = ch->getEvent();
    std::lock_guard<std::mutex> lock(_mutex);
    if (!ch->getInEpoll()) {
        errorExit(epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add fail!");
        ch->setInEpoll(true);
    } else {
        errorExit(epoll_ctl(_epfd, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll modify fail!");
    }
}
// 从epoll删除Channel
void Epoll::deleteChannel(Channel* ch) const {
    int fd = ch->getFd();
    if (fd != -1) {
        std::lock_guard<std::mutex> lock(_mutex);
        errorExit(epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, nullptr) == -1, "epoll delete fail!");
        ch->setInEpoll(false);
    }
}

std::vector<Channel*> Epoll::trigChannelSet(int timeout) const {
    std::vector<Channel*> trig_chs;
    LOG_INFO("listen epoll(epfd %d)", _epfd);
    int trignum = epoll_wait(_epfd, _evarr, MAX_EVENTS, timeout);
    errorExit(trignum == -1, "epoll wait fail!");
    for (int i = 0; i < trignum; ++i) {
        Channel* ch = static_cast<Channel*>(_evarr[i].data.ptr);
        ch->setReady(_evarr[i].events);
        trig_chs.push_back(ch);
        LOG_INFO("fd %d has event in epoll(epfd %d)", ch->getFd(), _epfd);
    }
    LOG_INFO("gain event set");
    return trig_chs;
}
