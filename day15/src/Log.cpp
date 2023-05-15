#include "Log.h"

std::ostream& Log::_out = std::cout;

#if BUILD_TYPE == LEVEL_DEBUG
Log::LogLevel Log::_levelEnv = LogLevel::DEBUG;
#elif BUILD_TYPE == LEVEL_INFO
Log::LogLevel Log::_levelEnv = LogLevel::INFO;
#else
Log::LogLevel Log::_levelEnv = LogLevel::DEBUG;
#endif

Log::Log() {}

Log::~Log() {}

void Log::setOutput(std::ostream& out) { _out.rdbuf(out.rdbuf()); }

void Log::printHead(const LogLevel& level) {
  switch (level) {  // 根据不同级别输出不同颜色的前缀（仅在Linux下有效）
    case DEBUG:
      _out << "\033[36m[DEBUG]\033[0m ";
      break;
    case INFO:
      _out << "\033[32m[INFO]\033[0m ";
      break;
    case WARN:
      _out << "\033[33m[WARN]\033[0m ";
      break;
    case ERROR:
      _out << "\033[31m[ERROR]\033[0m ";
      break;
    default:
      break;
  }
}
