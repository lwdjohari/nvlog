#pragma once

// #include <absl/synchronization/mutex.h>

#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "nvlog/concurrent_queue.h"
#include "nvlog/declare.h"

namespace nvlog {
class Sink {
 public:
  virtual ~Sink() = default;
  virtual void Log(const std::shared_ptr<LogMessage> log_message) = 0;
  virtual void Start() = 0;
  virtual void Shutdown(bool force = false) = 0;
  virtual bool IsRun() const = 0;
  void SetFormatter(void (*formatter)(std::ostringstream&, const LogMessage&)) {
    formatter_ = formatter;
  }

 protected:
  void (*formatter_)(std::ostringstream&, const LogMessage&) = nullptr;
};

class AsyncSink : public Sink {
 public:
  AsyncSink() : running_(false), prepare_shutdown_(false) {}

  ~AsyncSink() {}

  void Log(const std::shared_ptr<LogMessage> log_message) override {
    queue_.Enqueue(log_message);
  }

  virtual void Start() override {
    if (running_.load())
      return;
    running_ = true;
    total_ = 0;
    prepare_shutdown_.store(false);
    worker_thread_ = std::thread(&AsyncSink::Run, this);
  }

  virtual void Shutdown(bool force = false) override {
    if(!running_.load() || prepare_shutdown_.load())
      return;

    if (running_.load()) {
      prepare_shutdown_.store(true);
      // Enqueue a null to unblock the worker thread
      queue_.Enqueue(nullptr);  

      if (worker_thread_.joinable()) {
        worker_thread_.join();
      }
    }
  }

  virtual bool IsRun() const override {
    return running_.load();
  }

 protected:
  virtual void Process(const std::shared_ptr<LogMessage>& message) = 0;

 private:
  void Run() {
    while (running_.load()) {
      std::shared_ptr<LogMessage> log_message;
      queue_.WaitAndDequeue(log_message);
      if (prepare_shutdown_.load()) {
#if NVLOG_DEBUG == 1 && NVLOG_TRACE == 1
        std::cout << "Sink::WORKER_BREAK: " << queue_.Size() << " logs left."
                  << std::endl;
#endif
        break;
      }

      if (log_message) {
        Process(log_message);
        // total_++;
      }
    }

    if (prepare_shutdown_.load()) {
// Process remaining messages after running_ is set to false
#if NVLOG_DEBUG == 1 && NVLOG_TRACE == 1
      std::cout << "Sink::PREPARE SHUTDOWN: " << queue_.Size() << " logs left."
                << std::endl;
#endif
      while (!queue_.Empty()) {
        std::shared_ptr<LogMessage> log_message;

        queue_.TryDequeue(log_message);
        if (log_message) {
          Process(log_message);
        }
      }
#if NVLOG_DEBUG == 1 && NVLOG_TRACE == 1
      std::cout << "ConsoleSink::SHUTDOWN: " << queue_.Size() << " logs left."
                << std::endl;
#endif
    }

    running_.store(false);
  }

  ConcurrentQueue<std::shared_ptr<LogMessage>> queue_;
  std::thread worker_thread_;
  std::atomic<bool> running_;
  std::atomic<bool> prepare_shutdown_;
  std::atomic<int64_t> total_;
};
}  // namespace nvlog
