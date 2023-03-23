#include "EventLoop.h"

EventLoop::EventLoop() {
  _running = false;
  _ep = new Epoll();
  _threadPool = new ThreadPool();
}

EventLoop::~EventLoop() {
  stop();

  delete _ep;
  _ep = nullptr;

  delete _threadPool;
  _threadPool = nullptr;
}

void EventLoop::loop() {
  if (!_running) {
    Log::warn("EventLoop no start!");
  }
  while (_running) {
    std::vector<Channel*> channels = _ep->poll();
    for (auto channel : channels) {
      if (channel->isSync()) {
        channel->handle();
      } else {
        _threadPool->addTask([=]() { channel->handle(); });
      }
    }
  }
  Log::info("EventLoop loop end");
}

void EventLoop::updateChannel(Channel* channel) { _ep->updateChannel(channel); }
void EventLoop::deleteChannel(Channel* channel) { _ep->deleteChannel(channel); }
void EventLoop::start() {
  _running = true;
  _threadPool->start(5);
}

void EventLoop::stop() {
  if (_running) {
    _running = false;
    _threadPool->stop();
  }
}