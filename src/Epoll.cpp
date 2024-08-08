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
/*获取触发监听事件的事件集合*/
std::vector<Channel*> Epoll::triggerNodeSet(int timeout){   
    std::vector<Channel*> trig_chs;
    int trignum = epoll_wait(m_epfd, m_evarr, MAXEVENT, timeout);
    errorExit(trignum==-1,"gain trigger set fail!");
    for(int i=0; i<trignum; i++){
        Channel *ch = (Channel*)m_evarr[i].data.ptr;
        ch->setRevents(m_evarr[i].events);
        trig_chs.push_back(ch);
    }
    return trig_chs;
}
/*通过channel构建事件节点，并上epoll树*/
void Epoll::addChannel(Channel* channel){
    int fd = channel->getFd();
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data.ptr = channel;
    ev.events = channel->getEvents();
    // channel不在epoll上，就加到epoll上
    if(!channel->getInEpoll()){
        errorExit(epoll_ctl(m_epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add fail!");
        channel->setInEpoll();
    }else{
        errorExit(epoll_ctl(m_epfd, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll modify fail!");
    }
}
}