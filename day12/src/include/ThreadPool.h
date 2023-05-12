#pragma once

#include <condition_variable>
#include <functional>
#include <future>
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

  void start(size_t size=5);
  void stop();

  template <class F, class... Args>
  auto addTask(F&& f, Args&&... args)
      -> std::future<typename std::result_of<F(Args...)>::type>;
};

template <class F, class... Args>
auto ThreadPool::addTask(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type> {
  using return_type = typename std::result_of<F(Args...)>::type;

  auto task = std::make_shared<std::packaged_task<return_type()>>(
      std::bind(std::forward<F>(f), std::forward<Args>(args)...));

  std::future<return_type> res = task->get_future();
  {
    std::unique_lock<std::mutex> lock(_tasksMtx);
    if (!_running) {
      throw std::runtime_error("ThreadPool not running\n");
    };
    _tasks.emplace([task]() { (*task)(); });
  }
  _cv.notify_one();  // 通知一次条件变量
  return res;
}