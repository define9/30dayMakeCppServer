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

  std::function<void()> _callback;  // 事件回调

 public:
  /// @brief 构造通道
  /// @param fd 
  /// @param useSync 是否同步执行，默认false使用线程池处理
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
  void setCallback(std::function<void()> callback);
  void handle();

  void inETEvents();
  void inEvents();
};
