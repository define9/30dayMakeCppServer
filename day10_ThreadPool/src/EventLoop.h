#pragma once

#include <cstdio>
#include <vector>

#include "Channel.h"
#include "Epoll.h"
#include "ThreadPool.h"

class EventLoop {
 private:
  Epoll* _ep;
  volatile bool _running;
  ThreadPool* _threadPool;

 public:
  EventLoop();
  ~EventLoop();

  void loop();

  /**
   * 向 epoll 中更新 channel
   */
  void updateChannel(Channel* channel);
  void deleteChannel(Channel* channel);

  void start();
  void stop();
};
