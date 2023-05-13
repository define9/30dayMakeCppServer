#pragma once

#include <string>

class Buffer {
 private:
  std::string _buf;

 public:
  Buffer();
  ~Buffer();

  Buffer* append(const char* str, size_t size);
  Buffer* append(const std::string& str);
  size_t size();

  const char* c_str() const;
  void clear();
};
