#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <thread>

#include "macro.h"

namespace nvlog {
enum class LogLevel { Trace, Debug, Info, Warning, Error, Critical };

// Return current thread ID
// return: ```int```
inline std::string GetThreadId() {
  std::stringstream ss;
  ss << std::this_thread::get_id();
  return __NVL_RETURN_MOVE(ss.str());
}

// Hold date part from timestamp.
// Note: because time is platform dependent, if you see the source code time
// is using ```int``` to adapting the platform dependent size
struct DateTimeComponent {
  int year;
  int month;
  int day;
  int date;
  int hours;
  int minutes;
  int microseconds;

  static DateTimeComponent FromTimestamp(
      const std::chrono::system_clock::time_point& tp) {
    std::time_t time = std::chrono::system_clock::to_time_t(tp);
    std::tm tm = *std::localtime(&time);
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(
                            tp.time_since_epoch()) %
                        std::chrono::seconds(1);

    DateTimeComponent dtc;
    dtc.year = 1900 + tm.tm_year;
    dtc.month = 1 + tm.tm_mon;
    dtc.day = tm.tm_wday;
    dtc.date = tm.tm_mday;
    dtc.hours = tm.tm_hour;
    dtc.minutes = tm.tm_min;
    dtc.microseconds = static_cast<int>(microseconds.count());

    return __NVL_RETURN_MOVE(dtc);
  }

  void FormatYear(std::ostringstream& ss) const {
    ss << std::setw(4) << std::setfill('0') << year;
  }

  void FormatMonth(std::ostringstream& ss) const {
    ss << std::setw(2) << std::setfill('0') << month;
  }

  void FormatDay(std::ostringstream& ss) const {
    ss << std::setw(2) << std::setfill('0') << date;
  }

  void FormatHours(std::ostringstream& ss) const {
    ss << std::setw(2) << std::setfill('0') << hours;
  }

  void FormatMinutes(std::ostringstream& ss) const {
    ss << std::setw(2) << std::setfill('0') << minutes;
  }

  void FormatMicroseconds(std::ostringstream& ss) const {
    ss << std::setw(6) << std::setfill('0') << microseconds;
  }
};

// LogMessage struct
//
// Note:
// To turn ```ON``` and use the feature of ```custom data```
// Use NVLOG_UNSAFE_MODE = 1
// ```void* data``` Macro and function will be available
// Use at your own risks.
struct LogMessage {
  LogLevel log_level;
  std::chrono::system_clock::time_point timestamp;
  std::string tag;
  std::string message;
  std::string file;
  int32_t line;
  std::string thread_id;
  void* data;  // custom objects, unsafe do with considerations

  LogMessage(std::chrono::system_clock::time_point ts, LogLevel ll,
             std::string tg, std::string msg, std::string f, int32_t ln, std::string tid, void* d = nullptr)
                  : log_level(ll),
                    timestamp(ts),
                    tag(std::move(tg)),
                    message(std::move(msg)),
                    file(std::move(f)),
                    line(ln),
                    thread_id(std::move(tid)),
                    data(d) {}

  void FormatTimestamp(std::ostringstream& ss) const {
    std::time_t time = std::chrono::system_clock::to_time_t(timestamp);
    std::tm tm = *std::localtime(&time);
    std::chrono::microseconds us =
        std::chrono::duration_cast<std::chrono::microseconds>(
            timestamp.time_since_epoch()) %
        1000000;

    ss << std::put_time(&tm, "%Y%m%d %H:%M:%S") << '.' << std::setw(6)
       << std::setfill('0') << us.count();
  }
};



}  // namespace nvlog
