#pragma once

#include <memory>

#include "nvlog/channel.h"
#include "nvlog/console_sink.h"
#include "nvlog/declare.h"
#include "nvlog/limiters/rate_limiter.h"
namespace nvlog {

class Logger {
 public:
  explicit Logger(std::shared_ptr<limiters::RateLimiter> rate_limiter)
                  : channel_(std::make_shared<Channel>(rate_limiter)) {
    channel_->Start();
  }

  explicit Logger(std::shared_ptr<limiters::RateLimiter> rate_limiter,
                  std::vector<std::shared_ptr<nvlog::Sink>>& sinks)
                  : channel_(std::make_shared<Channel>(rate_limiter, sinks)) {}

  void StartEngine() {
    channel_->Start();
  }

  bool IsRun() const {
    return channel_->IsRun();
  }

  void ShutdownEngine(bool force = false) {
    channel_->Shutdown(force);
  }

  ~Logger() {}

  static void RegisterLogger(std::vector<std::shared_ptr<nvlog::Sink>>& sinks) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!instance_) {
      instance_ = std::make_shared<Logger>(
          std::make_shared<limiters::NullLimiter>(), sinks);
    }
  }

  static std::shared_ptr<Logger> Get() {
    std::lock_guard<std::mutex> lock(mutex_);
    return instance_;
  }

  void Log(LogLevel level, const std::string& message, const std::string& tag,
           const std::string& file, int line, void* data = nullptr) const {
    auto log_message = std::make_shared<LogMessage>(
        std::chrono::system_clock::now(), level, tag, message, file, line,
        GetThreadId(), data);
    channel_->Enqueue(log_message);
  }

  void AddSink(std::shared_ptr<Sink> sink) {
    channel_->AddSink(sink);
  }

 private:
  std::shared_ptr<Channel> channel_;
  static std::shared_ptr<Logger> instance_;
  static std::mutex mutex_;
};

#define LOG_TRACE(message)                                                 \
  nvlog::Logger::Get()->Log(nvlog::LogLevel::Trace, message, "", __FILE__, \
                            __LINE__);

#define LOG_DEBUG(message)                                                 \
  nvlog::Logger::Get()->Log(nvlog::LogLevel::Debug, message, "", __FILE__, \
                            __LINE__);

#define LOG_INFO(message)                                                 \
  nvlog::Logger::Get()->Log(nvlog::LogLevel::Info, message, "", __FILE__, \
                            __LINE__);

#define LOG_WARN(message)                                                    \
  nvlog::Logger::Get()->Log(nvlog::LogLevel::Warning, message, "", __FILE__, \
                            __LINE__);

#define LOG_ERROR(message)                                                 \
  nvlog::Logger::Get()->Log(nvlog::LogLevel::Error, message, "", __FILE__, \
                            __LINE__);

#define LOG_FATAL(message)                                                 \
  nvlog::Logger::Get()->Log(nvlog::LogLevel::Fatal, message, "", __FILE__, \
                            __LINE__);

// LOG Trace with tag
#define LOG_TRACE_T(message, tag)                                           \
  nvlog::Logger::Get()->Log(nvlog::LogLevel::Trace, message, tag, __FILE__, \
                            __LINE__);

// Log Debug with tag
#define LOG_DEBUG_T(message, tag)                                           \
  nvlog::Logger::Get()->Log(nvlog::LogLevel::Debug, message, tag, __FILE__, \
                            __LINE__);

#define LOG_INFO_T(message, tag)                                           \
  nvlog::Logger::Get()->Log(nvlog::LogLevel::Info, message, tag, __FILE__, \
                            __LINE__);

#define LOG_WARN_T(message, tag)                                              \
  nvlog::Logger::Get()->Log(nvlog::LogLevel::Warning, message, tag, __FILE__, \
                            __LINE__);

#define LOG_ERROR_T(message, tag)                                           \
  nvlog::Logger::Get()->Log(nvlog::LogLevel::Error, message, tag, __FILE__, \
                            __LINE__);

#define LOG_FATAL_T(message, tag)                                           \
  nvlog::Logger::Get()->Log(nvlog::LogLevel::Fatal, message, tag, __FILE__, \
                            __LINE__);

// LOG Trace with tag
#define LOG_TRACE_COND_T(statement, message, tag)                             \
  if (statement) {                                                            \
    nvlog::Logger::Get()->Log(nvlog::LogLevel::Trace, message, tag, __FILE__, \
                              __LINE__);                                      \
  }

// LOG Info with tag
#define LOG_INFO_COND_T(statement, message, tag)                             \
  if (statement) {                                                           \
    nvlog::Logger::Get()->Log(nvlog::LogLevel::Info, message, tag, __FILE__, \
                              __LINE__);                                     \
  }

// LOG Debug with tag
#define LOG_DEBUG_COND_T(statement, message, tag)                             \
  if (statement) {                                                            \
    nvlog::Logger::Get()->Log(nvlog::LogLevel::Debug, message, tag, __FILE__, \
                              __LINE__);                                      \
  }

// LOG Warning with tag
#define LOG_WARN_COND_T(statement, message, tag)                      \
  if (statement) {                                                    \
    nvlog::Logger::Get()->Log(nvlog::LogLevel::Warning, message, tag, \
                              __FILE__, __LINE__);                    \
  }

// LOG Error with tag
#define LOG_ERROR_COND_T(statement, message, tag)                             \
  if (statement) {                                                            \
    nvlog::Logger::Get()->Log(nvlog::LogLevel::Error, message, tag, __FILE__, \
                              __LINE__);                                      \
  }

// LOG Fatal with tag
#define LOG_FATAL_COND_T(statement, message, tag)                             \
  if (statement) {                                                            \
    nvlog::Logger::Get()->Log(nvlog::LogLevel::Fatal, message, tag, __FILE__, \
                              __LINE__);                                      \
  }
}  // namespace nvlog
