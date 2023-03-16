#include "EventLoop.h"

EventLoop::EventLoop() {
  _running = false;
  _ep = new Epoll();
}

EventLoop::~EventLoop() {
  _running = false;
  delete _ep;
  _ep = nullptr;
}

void EventLoop::loop() {
  if (!_running) {
    printf("[warn] no start\n");
  }
  while (_running) {
    std::vector<Channel*> channels = _ep->poll();
    for (auto channel : channels) {
      channel->handle();
    }
  }
  printf("loop end\n");
}

void EventLoop::updateChannel(Channel* channel) { _ep->updateChannel(channel); }

void EventLoop::start() { _running = true; }

void EventLoop::stop() {}