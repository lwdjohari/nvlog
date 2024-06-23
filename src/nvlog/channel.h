#pragma once

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "nvlog/concurrent_queue.h"
#include "nvlog/declare.h"
#include "nvlog/limiters/rate_limiter.h"
#include "nvlog/sink.h"

namespace nvlog {

class Channel {
 public:
  explicit Channel(std::shared_ptr<nvlog::limiters::RateLimiter> rate_limiter)
                  : rate_limiter_(rate_limiter), running_(false) {}

  explicit Channel(std::shared_ptr<nvlog::limiters::RateLimiter> rate_limiter,
                   std::vector<std::shared_ptr<nvlog::Sink>>& sinks)
                  : sinks_(sinks),
                    rate_limiter_(rate_limiter),
                    running_(false) {}

  ~Channel() {}

  void Start() {
    if (running_)
      return;
    running_ = true;
    for (const auto& sink : sinks_) {
      sink->Start();
    }
    worker_thread_ = std::thread(&Channel::Process, this);
  }

  bool IsRun() const {
    return running_.load();
  }

  void Shutdown(bool force) {
    if(!running_.load() || prepare_shutdown_.load())
      return;

    prepare_shutdown_ = true;
    queue_.Enqueue(nullptr);  // Enqueue a null to unblock the worker thread
    // sink have different rate for shutdown
    // 1 sink can goes black first without finished the log
    for (const auto& sink : sinks_) {
      // each sink already joined inside shutdown
      sink->Shutdown();
    }
    running_.store(false);
    queue_.Enqueue(nullptr);
    if (worker_thread_.joinable()) {
      worker_thread_.join();
    }
  }

  void Enqueue(const std::shared_ptr<LogMessage>& log_message) {
    if (!rate_limiter_ || rate_limiter_->Allow()) {
      queue_.Enqueue(log_message);
    }
  }

  void AddSink(std::shared_ptr<Sink> sink) {
    sinks_.push_back(sink);
  }

 private:
  void Process() {
    while (running_.load()) {
      std::shared_ptr<LogMessage> log_message;
      queue_.WaitAndDequeue(log_message);
      if (log_message) {
        for (const auto& sink : sinks_) {
          sink->Log(log_message);
        }
      }
      if (prepare_shutdown_.load())
        break;
    }

    while (prepare_shutdown_.load()) {
      if (!queue_.Empty()) {
        std::shared_ptr<LogMessage> log_message;
        queue_.TryDequeue(log_message);
        if (log_message) {
          for (const auto& sink : sinks_) {
            sink->Log(log_message);
          }
        }

        if (!running_.load()) {
          prepare_shutdown_.store(false);
#if  NVLOG_DEBUG == 1 && NVLOG_TRACE == 1
          std::cout << "Channel::Terminated" << std::endl;
#endif
          break;
        }
      }
    }
  }

  ConcurrentQueue<std::shared_ptr<LogMessage>> queue_;
  std::vector<std::shared_ptr<Sink>> sinks_;
  std::shared_ptr<nvlog::limiters::RateLimiter> rate_limiter_;
  std::thread worker_thread_;
  std::atomic<bool> running_;
  std::atomic<bool> prepare_shutdown_;
};

}  // namespace nvlog