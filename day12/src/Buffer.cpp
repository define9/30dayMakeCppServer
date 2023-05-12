#include "Buffer.h"

Buffer::Buffer() {}

Buffer::~Buffer() {}

Buffer* Buffer::append(const char* str, size_t size) {
  for (size_t i = 0; i < size; i++) {
    if (str[i] == '\0') break;
    _buf.push_back(str[i]);
  }
  return this;
}

Buffer* Buffer::append(const std::string& str) {
  _buf.append(str);
  return this;
}

size_t Buffer::size() { return _buf.size(); }

const char* Buffer::c_str(){
    return _buf.c_str();
}

void Buffer::clear() {
  _buf.clear();
}
