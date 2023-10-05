#include "EventLoop.h"

EventLoop::EventLoop() {
  _running = false;
  _ep = new Epoll();
  _threadPool = new ThreadPool();
}

EventLoop::~EventLoop() {
  stop();

  delete _ep;
  delete _threadPool;
}

void EventLoop::loop() {
  if (!_running) {
    Log::warn("EventLoop no start!");
  }
  while (_running) {
    std::vector<Channel*> channels = _ep->poll();

#if BUILD_TYPE == LEVEL_DEBUG
    std::string buf = "epoll channel fds: ";
#endif

    for (auto channel : channels) {
#if BUILD_TYPE == LEVEL_DEBUG
      buf.append(std::to_string(channel->getFd())).append(":")
        .append(std::to_string(channel->getRevents()).append(", "));
#endif

      if (channel->isSync()) {
        channel->handle();
      } else {
        _threadPool->addTask([=]() { channel->handle(); });
      }
    }
#if BUILD_TYPE == LEVEL_DEBUG
    Log::debug(buf);
#endif
  }
  Log::debug("loop end");
}

void EventLoop::updateChannel(Channel* channel) { _ep->updateChannel(channel); }
void EventLoop::deleteChannel(Channel* channel) { _ep->deleteChannel(channel); }
void EventLoop::start() {
  _running = true;
  _threadPool->start(5);
  Log::debug("start thread pool!");
}

void EventLoop::stop() {
  _running = false;
  _threadPool->stop();
  Log::debug("stop thread pool!");
}