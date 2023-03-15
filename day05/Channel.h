#pragma once

#include <unistd.h>
#include "Epoll.h"

class Epoll;

class Channel
{
private:
    Epoll* _ep;
    int _fd;
    uint32_t _events; // 希望监听的事件
    uint32_t _revents;// 正在发生的事件
    bool _inEpoll;

public:
    Channel(int fd, Epoll* ep);
    ~Channel();

    int getFd();
    uint32_t getEvents();
    uint32_t getRevents();
    bool getInEpoll();
    void addEpoll();

    void setRevents(uint32_t revents);

    void enableReading();
};
