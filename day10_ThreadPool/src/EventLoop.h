#pragma once

#include <cstdio>
#include <vector>

#include "Channel.h"
#include "Epoll.h"
#include "ThreadPool.h"

class EventLoop {
 private:
  Epoll* _ep;
  bool _running;
  ThreadPool* _threadPool;

 public:
  EventLoop();
  ~EventLoop();

  void loop();

  /**
   * 向 epoll 中更新 channel
   */
  void updateChannel(Channel* channel);

  void start();
  void stop();
};
