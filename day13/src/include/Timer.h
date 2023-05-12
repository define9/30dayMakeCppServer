#pragma once

#include <sys/timerfd.h>
#include <unistd.h>
#include 

#include "Log.h"

/**
 * 这里使用timerfd,由linux系统维护,每隔一段时间fd就是可读的
*/
class Timer {
 private:
  int _fd;


 public:
  Timer(long ms);
  ~Timer();
};

Timer::Timer(long ms) {
  // CLOCK_REALTIME  随系统时间改变
  // CLOCK_MONOTONIC 启动时开始计时，不随系统时间改变
  _fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
  itimerspec spec;
  // spec.it_value 第一次的运行时间
  spec.it_value.tv_nsec = ms * 1000;
  spec.it_value.tv_sec = 0;
  // spec.it_interval 后续的定时周期
  spec.it_interval.tv_nsec = ms * 1000;
  spec.it_interval.tv_sec = 0;

  int err = timerfd_settime(_fd, TFD_TIMER_ABSTIME, &spec, NULL);
  if (err < 0) {
    close(_fd);
  }
}

Timer::~Timer() {}
