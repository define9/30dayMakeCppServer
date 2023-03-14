#include <errno.h>
#include <stdio.h>

#include "Epoll.h"
#include "InetAddress.h"
#include "Socket.h"
#include "util.h"

#define READ_BUFFER 1024

void handleReadEvent(int);

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
  ep->addFd(socket->getFd(), EPOLLIN | EPOLLET);

  while (true) {
    std::vector<epoll_event> events = ep->poll();
    for (int i = 0; i < events.size(); i++) {
      if (events[i].data.fd == socket->getFd()) {
        if (events[i].events & EPOLLIN) {
          // 新建立一个连接, 这里有内存泄漏, 因为没有delete
          // 如果socket这里不用指针, 变量在这个if结束后,指针引用的将出问题
          // 但是clientIP只是用于绑定然后显示一下, 后面交互用的是 socketfd
          InetAddress clientIp;
          int fd = socket->accept(&clientIp);
          Socket* client = new Socket(fd);

          printf("new client fd %d! IP: %s Port: %d\n", client->getFd(), inet_ntoa(clientIp.addr.sin_addr), ntohs(clientIp.addr.sin_port));

          client->setnonblocking();
          ep->addFd(client->getFd(), EPOLLIN | EPOLLET);
        }
      } else if (events[i].events & EPOLLIN) {
        // 只是一个可读事件
        handleReadEvent(events[i].data.fd);
      } else {
        printf("no support event");
      }
    }
  }

  delete ep;
  delete socket;
}

void handleReadEvent(int fd) {
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
      break;
    }
  }
}