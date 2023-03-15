#pragma once

#include <unistd.h>
#include <functional>
#include "Epoll.h"

class Channel
{
private:
    int _fd;
    uint32_t _events;   // 希望监听的事件
    uint32_t _revents;  // 正在发生的事件
    bool _inEpoll;      // 是否已经在epoll中了

    std::function<void()> _callback; // 事件回调

public:
    Channel(int fd);
    ~Channel();

    int getFd();
    uint32_t getEvents();
    uint32_t getRevents();
    bool getInEpoll();
    void addEpoll();

    void setRevents(uint32_t revents);
    void setCallback(std::function<void()> callback);
    void handle();

    void prepareEvents();
};
