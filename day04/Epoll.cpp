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

std::vector<epoll_event> Epoll::poll(int timeout) {
    std::vector<epoll_event> activeEvents;
    int nfds = epoll_wait(_epfd, _events, MAX_EVENTS, timeout); //有nfds个事件
    errif(nfds < 0, "epoll_wait error");
    for (int i = 0; i < nfds; i++) {
        activeEvents.push_back(_events[i]);
    }
    return activeEvents;
}