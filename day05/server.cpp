#include <errno.h>
#include <stdio.h>

#include "Channel.h"
#include "Epoll.h"
#include "InetAddress.h"
#include "Socket.h"
#include "util.h"
#include <map>

#define READ_BUFFER 1024

void handleReadEvent(Socket*);
std::map<int, Socket*> existSocket;

int main() {
  // 1. 创建服务端socket, 和epoll
  Socket *socket = new Socket();
  Epoll *ep = new Epoll();

  // 2. 绑定socket地址
  InetAddress addr("0.0.0.0", 8888);
  socket->bind(&addr);
  socket->listen();
  printf("listen ...\n");

  // 3. 设置为非堵塞, 放入epoll中
  socket->setnonblocking();
  Channel servChannel(socket->getFd(), ep);
  // 监听可读事件,更新到epoll
  servChannel.enableReading();

  while (true) {
    std::vector<Channel*> channels = ep->poll();
    for (int i = 0; i < channels.size(); i++) {
      if (channels[i]->getFd() == socket->getFd()) {
        if (channels[i]->getRevents() & EPOLLIN) {
          // 新建立一个连接
          // 如果socket这里不用指针, 变量在这个if结束后,指针引用的将出问题
          // 但是clientIP只是用于绑定然后显示一下, 后面交互用的是 socket 的 fd
          InetAddress clientIp;
          int fd = socket->accept(&clientIp);
          Socket* client = new Socket(fd);
          existSocket.insert_or_assign(fd, client);

          printf("new client fd %d! IP: %s Port: %d\n", client->getFd(), inet_ntoa(clientIp.addr.sin_addr), ntohs(clientIp.addr.sin_port));
          printf("exist client count: %d\n", existSocket.size());

          client->setnonblocking(); // 设置client的socket为非堵塞
          
          // Channel 没有 delete, 内存泄漏
          Channel* channel = new Channel(client->getFd(), ep);
          channel->enableReading();
        }
      } else if (channels[i]->getRevents() & EPOLLIN) {
        // 只是一个可读事件
        auto it = existSocket.find(channels[i]->getFd());
        if (it != existSocket.end()) {
          handleReadEvent(it->second);
        }
      } else {
        printf("no support event");
      }
    }
  }

  delete ep;
  delete socket;
}

void handleReadEvent(Socket* socket) {
  int fd = socket->getFd();
  char buf[READ_BUFFER];
  while (true) {
    // 由于使用非阻塞IO, 需要循环读取, 到这里时,
    // 数据可能没有传输完全, 如果判断数据不完整, 则等待一下continue
    bzero(&buf, sizeof(buf));
    ssize_t bytes_read = read(fd, buf, sizeof(buf));
    if (bytes_read > 0) {
      printf("message from client fd %d: %s\n", fd, buf);
      write(fd, buf, sizeof(bytes_read));  // 客户端发的数据echo回去, 读多少回复多少
    } else if (bytes_read == -1 && errno == EINTR) {  // 客户端正常中断、继续读取
      printf("continue reading");
      continue;
    } else if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {  // 非阻塞IO，这个条件表示数据全部读取完毕
      printf("finish reading once, errno: %d\n", errno);
      break;
    } else if (bytes_read == 0) {  // EOF，客户端断开连接
      printf("EOF, client fd %d disconnected\n", fd);

      close(fd);  // 关闭socket会自动将文件描述符从epoll树上移除
      existSocket.erase(fd);
      delete socket;

      break;
    }
  }
}