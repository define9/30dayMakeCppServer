#pragma once
#include <sys/socket.h>//创建socket
#include "util.h"
#include <fcntl.h>//设置非阻塞
#include <arpa/inet.h>
#include <unistd.h>
#include "InetAddress.h"

class Socket
{
private:
    int _fd;
public:
    Socket();
    Socket(int);
    ~Socket();

    void bind(InetAddress*);
    void listen();
    void setnonblocking();

    int accept(InetAddress*);

    int getFd();
};

