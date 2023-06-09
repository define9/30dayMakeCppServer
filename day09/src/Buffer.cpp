#include "Buffer.h"

Buffer::Buffer() {}

Buffer::~Buffer() {}

void Buffer::append(const char* str, size_t size) {
  for (size_t i = 0; i < size; i++) {
    if (str[i] == '\0') break;
    _buf.push_back(str[i]);
  }
}

size_t Buffer::size() { return _buf.size(); }

const char* Buffer::c_str(){
    return _buf.c_str();
}

void Buffer::clear() {
  _buf.clear();
}
