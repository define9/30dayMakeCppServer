#include "Epoll.h"

int Epoll::ErrorCount = 0;

Epoll::Epoll() {
  _epfd = guard(epoll_create(1), "epoll create error");
  _events = new epoll_event[MAX_EVENTS];
  bzero(_events, sizeof(*_events) * MAX_EVENTS);
}

Epoll::~Epoll() {
  if (_epfd >= 0) {
    close(_epfd);
    _epfd = -1;
  }
  delete[] _events;
}

std::vector<Channel*> Epoll::poll(int timeout) {
  std::vector<Channel*> activeEvents;
  int nfds = epoll_wait(_epfd, _events, MAX_EVENTS, timeout);  // 有nfds个事件
  errif(nfds < 0, "epoll_wait error");
  for (int i = 0; i < nfds; i++) {
    Channel* channel = (Channel*)_events[i].data.ptr;
    channel->setRevents(_events[i].events);  // 设置正在发生的事件
    activeEvents.push_back(channel);
  }
  return activeEvents;
}

void Epoll::updateChannel(Channel* channel) {
  int fd = channel->getFd();  // 拿到Channel的文件描述符

  struct epoll_event ev;
  bzero(&ev, sizeof(ev));
  ev.data.ptr = channel;
  ev.events = channel->getEvents();
  if (!channel->getInEpoll()) {
    errif(epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev) < 0, "epoll add error");
    channel->addToEpoll();
  } else {
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!modify epoll\n");
    errif(epoll_ctl(_epfd, EPOLL_CTL_MOD, fd, &ev) < 0, "epoll modify error");
  }
}

void Epoll::deleteChannel(Channel* channel) {
  int fd = channel->getFd();
  errif(epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, nullptr) < 0, "epoll add error");
  channel->addToEpoll();
}