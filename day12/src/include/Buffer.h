#pragma once

#include <string>

class Buffer {
 private:
  std::string _buf;

 public:
  Buffer();
  ~Buffer();

  void append(const char* str, size_t size);
  size_t size();

  const char* c_str();
  void clear();
};
