#pragma once
#include <signal.h>
#include <functional>
#include <map>

std::map<int, std::function<void()>> handlers;
void signal_handler(int sig) {
  handlers[sig]();
}

struct Signal {
  static void signal(int sig, const std::function<void()> &handler) {
    handlers[sig] = handler;
    ::signal(sig, signal_handler);
  }
};