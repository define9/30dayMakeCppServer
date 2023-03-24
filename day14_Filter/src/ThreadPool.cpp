#include "ThreadPool.h"

ThreadPool::ThreadPool() {}

ThreadPool::~ThreadPool() { stop(); }

void ThreadPool::work(int i) {
  while (true) {
    std::function<void()> task;
    {  // 在这个{}作用域内对std::mutex加锁，出了作用域会自动解锁，不需要调用unlock()
      std::unique_lock<std::mutex> lock(_tasksMtx);
      _cv.wait(lock,
               [=]() {  // 等待条件变量，条件为任务队列不为空或线程池停止
                 return !_running || !_tasks.empty();
               });
      if (!_running && _tasks.empty()) {
        return;  // 任务队列为空并且线程池停止，退出线程
      }
      task = _tasks.front();  // 从任务队列头取出一个任务
      _tasks.pop();
    }
    task();  // 执行任务
  }
}

void ThreadPool::start(size_t size) {
  _running = true;

  for (size_t i = 0; i < size; i++) {
    _threads.emplace_back(new std::thread([=]() { ThreadPool::work(i); }));
  }
  Log::debug("ThreadPool started, thread count: ", size);
}

void ThreadPool::stop() {
  if (_running) {
    _running = false;
    _cv.notify_all();
    for (size_t i = 0; i < _threads.size(); i++) {
      if (_threads[i]->joinable()) _threads[i]->join();
      delete _threads[i];
    }
    _threads.clear();
    Log::debug("ThreadPool stop");
  }
}
