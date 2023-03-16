#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
 private:
  std::vector<std::thread*> _threads;
  std::queue<std::function<void()>> _tasks;
  std::mutex _tasksMtx;
  std::condition_variable _cv;
  volatile bool _running;

  void work(int i);

 public:
  ThreadPool();
  ~ThreadPool();

  void addTask(std::function<void()>);
  void start(size_t size=5);
  void stop();
};
