#pragma once
#include <errno.h>

#include <map>
#include <mutex>
#include <sstream>

#include "Acceptor.h"
#include "Connection.h"
#include "Dispatcher.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Log.h"
#include "Socket.h"
#include "model/IController.h"
#include "model/IFilter.h"
#include "model/Request.h"
#include "model/Response.h"

#define READ_BUFFER 1024

class Server {
 private:
  EventLoop* _loop{nullptr};
  Acceptor* _acceptor{nullptr};
  Dispatcher* _dispatcher{nullptr};

  InetAddress _serverAddr;
  std::map<int, Connection*> _openConnection;
  std::mutex _mapLock;

  std::vector<IFilter*> _filters;

  void init();

 public:
  Server(const char* ip, uint16_t port);
  Server(uint16_t port);
  ~Server();

  void loop();
  void stop();

  void newConnection(Connection* conn);
  void disConnection(Connection* conn);

  /// @brief 添加filter, 并由Server负责delete
  /// @param filter
  void registerFilter(IFilter* filter);
  void registerController(IController* controller);

  void addHandle(std::string url, std::function<void(const Request* req, Response* resp)> handle);
};
