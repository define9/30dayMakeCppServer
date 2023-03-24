#include "util.h"

void errif(bool condition, const char *errmsg) {
  if (condition) {
    perror(errmsg);
    exit(EXIT_FAILURE);
  }
}

int guard(int n, const char *err) {
  if (n == -1) {
    perror(err);
    exit(1);
  }
  return n;
}

bool socketIsOpen(int fd) {
  int error;
  socklen_t len = sizeof(error);

  int ret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len);
  if (ret == -1) {
    Log::error("getsockopt failed");
    return false;
  }
  return ret == 0;
}

bool connectStatus(int res) {
  if (res == 0) {  // 连接成功
    return true;
  } else if (errno == EINPROGRESS) {
    std::cout << "nonblock" << std::endl;
  } else {  // 连接失败
    std::cerr << "Error in connect(). errno: " << errno << std::endl;
    if (errno == ECONNREFUSED) {
      std::cerr << "Connection refused." << std::endl;
    } else if (errno == ETIMEDOUT) {
      std::cerr << "Connection timed out." << std::endl;
    } else if (errno == ENETUNREACH) {
      std::cerr << "Network unreachable." << std::endl;
    } else if (errno == EHOSTUNREACH) {
      std::cerr << "Host unreachable." << std::endl;
    }
  }
  return false;
}

std::vector<std::string> stringSplit(const std::string &str, char delim) {
  std::size_t previous = 0;
  std::size_t current = str.find(delim);
  std::vector<std::string> elems;
  while (current != std::string::npos) {
    if (current > previous) {
      elems.push_back(str.substr(previous, current - previous));
    }
    previous = current + 1;
    current = str.find(delim, previous);
  }
  if (previous != str.size()) {
    elems.push_back(str.substr(previous));
  }
  return elems;
}

std::string getSuffix(const std::string &filename) {
  return filename.substr(filename.find_last_of('.'));
}
