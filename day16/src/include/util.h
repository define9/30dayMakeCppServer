#ifndef UTIL_H
#define UTIL_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <map>
#include <vector>

#include "Log.h"
#include "Socket.h"

void errif(bool, const char*);
int guard(int, const char*);

bool socketIsOpen(int fd);

bool connectStatus(int res);

/// @brief socket 允许复用
/// @param fd
void socketReuseAddr(int fd);

std::vector<std::string> stringSplit(const std::string& str, char delim);

std::string getSuffix(const std::string& filename);

bool startWith(const std::string& str, const std::string& prefix);
bool startWith(const std::string& str, char prefix);

template <typename Key, typename Value>
auto safeGet(const std::map<Key, Value>& map, const Key key, const Value def) {
  auto it = map.find(key);
  Value res;
  if (it == map.end()) {
    res = def;
  } else {
    res = it->second;
  }
  return res;
}

#endif