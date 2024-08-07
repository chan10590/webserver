#include "Epoll.h"
#include "../util/util.h"
#include <string.h>
#include <unistd.h>
#define MAXEVENT 1024
namespace web{
Epoll::Epoll():m_epfd(-1), m_evarr(new epoll_event[MAXEVENT]){
    m_epfd = epoll_create1(0);
    errorExit(m_epfd==-1, "epoll create fail!");
    memset(m_evarr, 0, sizeof(*m_evarr)*MAXEVENT);
}

Epoll::~Epoll(){
    if(m_epfd != -1){
        close(m_epfd);
        m_epfd = -1;
    }
    delete[] m_evarr;
    memset(m_evarr, 0, sizeof(*m_evarr)*MAXEVENT);
}
/*设置fd的监听事件，并上epoll树*/
void Epoll::addNode(int fd, uint32_t ev_events){
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = ev_events;
    errorExit(epoll_ctl(m_epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add fail!");
}
/*获取触发监听事件的事件集合*/
std::vector<epoll_event*> Epoll::triggerNodeSet(int timeout){
    std::vector<epoll_event*> trig_evs;
    int trignum = epoll_wait(m_epfd, m_evarr, MAXEVENT, timeout);
    errorExit(trignum==-1,"gain trigger set fail!");
    for(int i=0; i<trignum; i++){
        trig_evs.emplace_back(&m_evarr[i]);
    }
    return trig_evs;
}
}