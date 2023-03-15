#pragma once
#include <sys/epoll.h>
#include <unistd.h>

#include <cstring>
#include <vector>

#include "Channel.h"
#include "util.h"

#define MAX_EVENTS 1000

class Channel;

class Epoll {
 private:
  int _epfd;
  struct epoll_event* _events;

 public:
  Epoll();
  ~Epoll();

  void addFd(int fd, uint32_t op);
  std::vector<Channel*> poll(int timeout = -1);
  /**
   * @brief 更新channel
   */
  void updateChannel(Channel* channel);
};
