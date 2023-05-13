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
  bool cancel;
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
  Timer(long sec = 1);
  ~Timer();

  Channel* getChannel();

  void tick();
  /// @brief 返回Job的引用
  /// @param  几秒后执行
  /// @param  执行的回调
  /// @return Job的引用,可以用于删除任务
  Job* addTask(long, std::function<void()>);
  void delTask(Job* job);
};
