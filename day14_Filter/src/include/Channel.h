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
  std::function<bool()> _read_callback;
  std::function<void()> _write_callback;

 public:
  Channel(int fd, bool useSync=true);
  ~Channel();

  int getFd();
  uint32_t getEvents();
  uint32_t getRevents();
  bool isSync();
  bool getInEpoll();
  void addToEpoll();
  void deleteFromEpoll();

  void setRevents(uint32_t revents);
  
  /**
   * @brief 这里有点耦合, 关闭连接也会在读取里面, 所以, 返回是否关闭了连接, 不在调用write回写
  */
  void setReadCallback(std::function<bool()> read);
  void setWriteCallback(std::function<void()> write);
  void handle();

  void inETEvents();
  void inEvents();
};
