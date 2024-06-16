#pragma once

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace nvlog {

inline std::string FormatTimestamp(
    const std::chrono::system_clock::time_point& tp) {
  std::time_t time = std::chrono::system_clock::to_time_t(tp);
  std::tm tm = *std::localtime(&time);
  std::chrono::microseconds us =
      std::chrono::duration_cast<std::chrono::microseconds>(
          tp.time_since_epoch()) %
      1000000;

  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y%m%d %H:%M:%S") << '.' << std::setw(6)
      << std::setfill('0') << us.count();
  return oss.str();
}
}  // namespace nvlog
