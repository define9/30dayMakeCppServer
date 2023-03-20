#pragma once

#include <iostream>
#include <string>

#include "build_type.hpp"

/// @brief 先不考虑多线程的输出
class Log {
 public:
  enum LogLevel { DEBUG, INFO, WARN, ERROR };

 private:
  static Log* _instance;
  static std::ostream& _out;
  static LogLevel _levelEnv;  // 当前环境的等级
  Log();

  static void printHead(const LogLevel& level);

  template <typename T, typename... Args>
  static void print(const T& t, Args... args);

  template <typename T>
  static void print(const T& arg);

 public:
  ~Log();
  static Log* getInstance();
  static void setOutput(std::ostream& out);

  template <typename T, typename... Args>
  static void debug(const T& firstArg, Args... args);

  template <typename T, typename... Args>
  static void info(const T& firstArg, Args... args);

  template <typename T, typename... Args>
  static void warn(const T& firstArg, Args... args);

  template <typename T, typename... Args>
  static void error(const T& firstArg, Args... args);
};

template <typename T, typename... Args>
void Log::debug(const T& firstArg, Args... args) {
  if (_levelEnv <= LogLevel::DEBUG) {
    printHead(LogLevel::DEBUG);
    print(firstArg, args...);
  }
}

template <typename T, typename... Args>
void Log::info(const T& firstArg, Args... args) {
  if (_levelEnv <= LogLevel::INFO) {
    printHead(LogLevel::INFO);
    print(firstArg, args...);
  }
}

template <typename T, typename... Args>
void Log::warn(const T& firstArg, Args... args) {
  if (_levelEnv <= LogLevel::WARN) {
    printHead(LogLevel::WARN);
    print(firstArg, args...);
  }
}

template <typename T, typename... Args>
void Log::error(const T& firstArg, Args... args) {
  if (_levelEnv <= LogLevel::ERROR) {
    printHead(LogLevel::ERROR);
    print(firstArg, args...);
  }
}

template <typename T>
void Log::print(const T& arg) {
  _out << arg << std::endl;
}
template <typename T, typename... Args>
void Log::print(const T& firstArg, Args... args) {
  _out << firstArg;
  print(args...);
}
