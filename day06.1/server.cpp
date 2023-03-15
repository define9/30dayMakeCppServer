#include <errno.h>
#include <stdio.h>

#include "src/Channel.h"
#include "src/EventLoop.h"
#include "src/InetAddress.h"
#include "src/Socket.h"
#include "src/util.h"
#include <map>

#define READ_BUFFER 1024

void handleReadEvent(Socket* socket, Channel* channel);
void newConnection(Socket* servSocket, EventLoop* loop);
// std::map<int, Socket*> existSocket;

int main() {
  // 1. 创建服务端socket, 和eventloop
  Socket* socket = new Socket();
  EventLoop* loop = new EventLoop();

  // 2. 绑定socket地址
  InetAddress addr("0.0.0.0", 8888);
  socket->bind(&addr);
  socket->listen();
  printf("listen ...\n");

  // 3. 设置为非堵塞, 放入epoll中
  printf("build channel.\n");
  socket->setnonblocking();
  Channel servChannel(socket->getFd());
  servChannel.prepareEvents();
  // 4. 对于 服务socket的回调函数, 是新增一个连接
  servChannel.setCallback([&](){
    newConnection(socket, loop);
  });
  printf("build channel done.\n");
  loop->updateChannel(&servChannel);

  loop->start();
  loop->loop();

  delete loop;
  delete socket;
}

void newConnection(Socket* servSocket, EventLoop* loop) {
  // 1. 声明地址, 接收客户端连接
  InetAddress clientIp;
  int fd = servSocket->accept(&clientIp);
  Socket* client = new Socket(fd);
  client->setnonblocking();

  printf("new client fd %d! IP: %s Port: %d\n", client->getFd(), inet_ntoa(clientIp.addr.sin_addr), ntohs(clientIp.addr.sin_port));

  // 2. 构造 Channel, 放入 Eventloop
  Channel* channel = new Channel(fd);
  channel->prepareEvents();
  // 3. 对于(新增连接)客户端的回调, 也就是读取事件
  channel->setCallback([=](){
    handleReadEvent(client, channel);
  });
  loop->updateChannel(channel);
}

void handleReadEvent(Socket* socket, Channel* channel) {
  int fd = socket->getFd();
  printf("read event fd: %d", fd);
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

      close(fd);// 关闭socket会自动将文件描述符从epoll树上移除
      // existSocket.erase(fd);
      delete socket; // 删除客户端
      delete channel;

      break;
    }
  }
}