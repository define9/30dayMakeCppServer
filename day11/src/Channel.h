#pragma once

#include <unistd.h>

#include <functional>

#include "Epoll.h"

class Channel {
 private:
  int _fd;
  uint32_t _events;   // 希望监听的事件
  uint32_t _revents;  // 正在发生的事件
  bool _inEpoll;      // 是否已经在epoll中了
  bool _useSync;

  // 事件回调
  std::function<void()> _read_callback;
  std::function<void()> _write_callback;

 public:
  Channel(int fd, bool useSync=false);
  ~Channel();

  int getFd();
  uint32_t getEvents();
  uint32_t getRevents();
  bool isSync();
  bool getInEpoll();
  void addToEpoll();
  void deleteFromEpoll();

  void setRevents(uint32_t revents);
  void setReadCallback(std::function<void()> read);
  void setWriteCallback(std::function<void()> write);
  void handle();

  void inETEvents();
  void inEvents();
};
