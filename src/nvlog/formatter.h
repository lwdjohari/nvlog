#pragma once

#include <sstream>

#include "nvlog/declare.h"

namespace nvlog {
inline void DefaultLevelFormatter(std::ostringstream& buffer,
                                  const LogLevel& level) {
  switch (level) {
    case LogLevel::Trace:
      buffer << std::left << std::setw(8) << std::setfill(' ') << "TRACE";
      break;
    case LogLevel::Debug:
      buffer << std::left << std::setw(8) << std::setfill(' ') << "DEBUG";
      break;
    case LogLevel::Info:
      buffer << std::left << std::setw(8) << std::setfill(' ') << "INFO";
      break;
    case LogLevel::Warning:
      buffer << std::left << std::setw(8) << std::setfill(' ') << "WARN";
      break;
    case LogLevel::Error:
      buffer << std::left << std::setw(8) << std::setfill(' ') << "ERROR";
      break;
    case LogLevel::Fatal:
      buffer << std::left << std::setw(8) << std::setfill(' ') << "CRITICAL";
      break;
    default:
      break;
  }

  buffer << std::resetiosflags(std::ios::adjustfield | std::ios::basefield |
                               std::ios::floatfield)
         << std::setfill('0');
}

inline void DefaultFormatter(std::ostringstream& buffer,
                             const LogMessage& log_message) {
  auto time = log_message.timestamp;
  auto time_t = std::chrono::system_clock::to_time_t(time);
  auto tm = *std::localtime(&time_t);
  auto ms = std::chrono::duration_cast<std::chrono::microseconds>(
                time.time_since_epoch()) %
            1000000;

  // clang-format off
  DefaultLevelFormatter(buffer, log_message.log_level);
  buffer 
      << std::setw(4) << std::setfill('0') << (1900 + tm.tm_year) << "-"
      << std::setw(2) << std::setfill('0') << (1 + tm.tm_mon) << "-"
      << std::setw(2) << tm.tm_mday
      << ' '
      << std::setw(2) << tm.tm_hour << ':'
      << std::setw(2) << tm.tm_min << ':'
      << std::setw(2) << tm.tm_sec << '.'
      << std::setw(6) << ms.count()
      << ' '
      << std::setw(5) << std::setfill(' ') << log_message.thread_id << std::setfill('0')
      << ' '
      << log_message.file << ':' << log_message.line << "]\n" 
      << std::left << std::setw(8) << std::setfill(' ') << " " 
      << "[" << log_message.tag << "] " << log_message.message;
  // clang-format on

  buffer << std::resetiosflags(std::ios::adjustfield | std::ios::basefield |
                               std::ios::floatfield)
         << std::setfill('0');
}

inline void SimpleFormatter(std::ostringstream& buffer,
                             const nvlog::LogMessage& log_message) {
  auto time = log_message.timestamp;
  auto time_t = std::chrono::system_clock::to_time_t(time);
  auto tm = *std::localtime(&time_t);
  auto ms = std::chrono::duration_cast<std::chrono::microseconds>(
                time.time_since_epoch()) %
            1000000;

  // clang-format off
  buffer <<"[";
  nvlog::DefaultLevelFormatter(buffer, log_message.log_level);
  buffer 
      << "] " 
      << std::setw(4) << std::setfill('0') << (1900 + tm.tm_year) << "-"
      << std::setw(2) << std::setfill('0') << (1 + tm.tm_mon) << "-"
      << std::setw(2) << tm.tm_mday
      << ' '
      << std::setw(2) << tm.tm_hour << ':'
      << std::setw(2) << tm.tm_min << ':'
      << std::setw(2) << tm.tm_sec << '.'
      << std::setw(6) << ms.count() << " "
      << "[" << log_message.tag << "] " << log_message.message;
  // clang-format on

  buffer << std::resetiosflags(std::ios::adjustfield | std::ios::basefield |
                               std::ios::floatfield)
         << std::setfill('0');
}
}  // namespace nvlog
