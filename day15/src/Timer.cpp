#include "Timer.h"

namespace {
u_int8_t scale[3] = {60, 60, 24};

}  // namespace

/// @brief 初始化定时器
/// @param sec 定时器的时间刻度
Timer::Timer(long sec) {
  // CLOCK_REALTIME  随系统时间改变
  // CLOCK_MONOTONIC 启动时开始计时，不随系统时间改变
  _fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
  itimerspec spec;
  // spec.it_value 第一次的运行时间
  spec.it_value.tv_nsec = 0;
  spec.it_value.tv_sec = 1;
  // spec.it_interval 后续的定时周期
  spec.it_interval.tv_nsec = 0;
  spec.it_interval.tv_sec = sec;

  int err = timerfd_settime(_fd, TFD_TIMER_ABSTIME, &spec, NULL);
  if (err < 0) {
    Log::error("init timer error");
    close(_fd);
  }
}

Timer::~Timer() {}

void Timer::next(u_int8_t i) {
  if (i > 2) {
    return;
  }
  p[i] = (p[i] + 1) % scale[i];
  if (p[i] == 0) {
    if (i == 2) {
      p[0] = p[1] = p[2] = 0;
    } else {
      next(i + 1);
    }
    jobUpgrade(i + 1);
  }
}

void Timer::jobUpgrade(u_int8_t i) {
  Log::debug("start upgrade job");
  if (i == 0) {
    return;
  }

  u_int8_t targetIndex = i - 1;
  for (auto& js : jobs[targetIndex]) {
    js.clear();
  }

  auto& jobVec = jobs[i][p[i]];
  Log::debug("need upgrade job size: ", jobVec.size());

  for (auto& job : jobVec) {
    jobs[targetIndex][job.time[targetIndex]].emplace_back(job);
    Log::debug("move job to ", job.time[targetIndex]);
  }

  Log::debug("job upgrade done.");
}

Channel* Timer::getChannel() {
  Channel* channel = new Channel(_fd);
  channel->inETEvents();
  channel->setCallback([=]() {
    unsigned long long exp;  // 读出数据,不然不切换状态epoll不会再次激活
    if (read(_fd, &exp, sizeof(exp)) != sizeof(exp))
      printf("read err, %d\n", errno);
    tick();
  });
  return channel;
}

void Timer::tick() {
  next(0);  // 时间轮自增

  // 只执行秒轮盘上的任务
  for (auto& job : jobs[0][p[0]]) {
    if (!job.cancel) {
      job.callback();
    }
  }
}

Job* Timer::addTask(long sec, std::function<void()> cb) {
  // 先不加锁
  // 3661 = 60 * 60 + 60 + 1
  int th = sec / (60 * 60);
  Job job;
  job.time[0] = p[0] + sec % 60;
  job.time[1] = p[1] + (sec - th * 60 * 60) / 60;
  job.time[2] = p[2] + th;
  job.callback = cb;
  job.format();
  Job* reference;

  for (int i = 2; i >= 0; i--) {
    if (job.time[i] != p[i]) {
      reference = &jobs[i][job.time[i]].emplace_back(job);
      Log::debug("add job to ", (int)job.time[i], " i: ", i);
      break;
    }
  }
  return reference;
}

void Job::format() {
  int l = 0;
  for (size_t i = 0; i <= 2; i++) {
    time[i] += l;
    l = 0;
    while (time[i] >= scale[i]) {
      time[i] -= scale[i];
      l++;
    }
  }
}

void Timer::delTask(Job* job) { job->cancel = true; }
