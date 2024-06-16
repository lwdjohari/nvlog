#pragma once

#include <chrono>
#include <fstream>
#include <iostream>

#include "nvlog/formatter.h"
#include "nvlog/sink.h"

namespace nvlog {
class DeferredFileSink : public AsyncSink {
 public:
  explicit DeferredFileSink(
      const std::string& filename, size_t max_buffer_size = 4096,
      std::chrono::seconds flush_interval = std::chrono::seconds(5))
                  : file_(filename, std::ios::out | std::ios::app),
                    flush_buffer_size_(max_buffer_size),
                    max_interval_(flush_interval) {
    if (!file_.is_open()) {
      LogMessage log(
          std::chrono::system_clock::now(), LogLevel::Error, std::move("nvlog"),
          std::move("DeferredFileSink failed to open log file: " + filename),
          __FILE__, __LINE__, GetThreadId(), nullptr);

      std::ostringstream ss;
      DefaultFormatter(ss, log);
      std::cerr << ss.str() << std::endl;
    }
    last_flush_time_ = std::chrono::steady_clock::now();
  }

  ~DeferredFileSink() {
    FlushBuffer();
    if (file_.is_open()) {
      file_.close();
    }
  }

 protected:
  void Process(const std::shared_ptr<LogMessage>& log_message) override {
    std::ostringstream ss;
    if (formatter_) {
      formatter_(ss, *log_message)
    } else {
      DefaultFormatter(ss, *log_message);
    }
    {
      std::lock_guard<std::mutex> lock(buffer_mutex_);
      buffer_ += ss.str() + "\n";
      if (buffer_.size() >= flush_buffer_size_ ||
          std::chrono::steady_clock::now() - last_flush_time_ >=
              max_interval_) {
        FlushBuffer();
      }
    }
  }

 private:
  std::ofstream file_;
  size_t flush_buffer_size_;
  std::chrono::seconds max_interval_;
  std::string buffer_;
  std::chrono::steady_clock::time_point last_flush_time_;
  std::mutex buffer_mutex_;

  void FlushBuffer() {
    std::lock_guard<std::mutex> lock(buffer_mutex_);
    if (!buffer_.empty()) {
      file_ << buffer_;
      file_.flush();
      buffer_.clear();
      last_flush_time_ = std::chrono::steady_clock::now();
    }
  }
};

}  // namespace nvlog
