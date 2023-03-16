#include "EventLoop.h"

EventLoop::EventLoop() {
  _running = false;
  _ep = new Epoll();
  _threadPool = new ThreadPool();
}

EventLoop::~EventLoop() {
  _running = false;

  delete _ep;
  _ep = nullptr;

  delete _threadPool;
  _threadPool = nullptr;
}

void EventLoop::loop() {
  if (!_running) {
    printf("[warn] no start!\n");
  }
  _threadPool->start(5);
  printf("start thread pool!\n");
  while (_running) {
    std::vector<Channel*> channels = _ep->poll();
    for (auto channel : channels) {
      _threadPool->addTask([=]() { channel->handle(); });
    }
  }
  printf("loop end\n");
}

void EventLoop::updateChannel(Channel* channel) { _ep->updateChannel(channel); }

void EventLoop::start() { _running = true; }

void EventLoop::stop() { _threadPool->stop(); }