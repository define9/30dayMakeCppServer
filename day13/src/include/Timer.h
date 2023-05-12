#pragma once

#include <sys/timerfd.h>
#include <unistd.h>
#include <functional>
#include <vector>

#include "Channel.h"
#include "Log.h"

class Channel;
class Job {
public:
  u_int8_t time[3]; // 0是秒，1是分，2是时
  std::function<void()> callback;

  void format();
};
/**
 * 这里使用timerfd,由linux系统维护,每隔一段时间fd就是可读的
*/
class Timer {
 private:
  int _fd;
  u_int8_t p[3]; // 时间轮当前时间
  std::vector<Job> jobs[3][60]; // 任务

  void next(u_int8_t i);
  void jobUpgrade(u_int8_t i);

 public:
  Timer(long ms);
  ~Timer();

  Channel* getChannel();

  void tick();
  void addTask(long, std::function<void()>);
};
