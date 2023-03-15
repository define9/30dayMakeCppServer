#include "Epoll.h"

Epoll::Epoll() {
    _epfd = guard(epoll_create(1), "epoll create error");
    _events = new epoll_event[MAX_EVENTS];
    bzero(_events, sizeof(*_events) * MAX_EVENTS);
}

Epoll::~Epoll() {
    if(_epfd >= 0){
        close(_epfd);
        _epfd = -1;
    }
    delete [] _events;
}

void Epoll::addFd(int fd, uint32_t op) {
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.fd = fd;
    ev.events = op;
    errif(epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev) < 0, "add epoll error"); // 添加到epoll
}

std::vector<Channel*> Epoll::poll(int timeout) {
    std::vector<Channel*> activeEvents;
    int nfds = epoll_wait(_epfd, _events, MAX_EVENTS, timeout); //有nfds个事件
    errif(nfds < 0, "epoll_wait error");
    for (int i = 0; i < nfds; i++) {
        Channel* channel = (Channel*) _events[i].data.ptr;
        channel->setRevents(_events[i].events);
        activeEvents.push_back(channel);
    }
    return activeEvents;
}

void Epoll::updateChannel(Channel* channel) {
    int fd = channel->getFd();  //拿到Channel的文件描述符
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.ptr = channel;
    ev.events = channel->getEvents();   //拿到Channel希望监听的事件
    if(!channel->getInEpoll()){
        errif(epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev) < 0, "epoll add error");//添加Channel中的fd到epoll
        channel->addEpoll();
    } else{
        errif(epoll_ctl(_epfd, EPOLL_CTL_MOD, fd, &ev) < 0, "epoll modify error");//已存在，则修改
    }
}