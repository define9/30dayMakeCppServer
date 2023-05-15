#pragma once

#include <cstdio>
#include <vector>

#include "Channel.h"
#include "Epoll.h"
#include "Log.h"
#include "ThreadPool.h"
#include "Timer.h"

class EventLoop {
 private:
  Epoll* _ep;
  volatile bool _running;
  ThreadPool* _threadPool;
  Timer* _timer;

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
