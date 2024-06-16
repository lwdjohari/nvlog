#pragma once

#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <atomic>
namespace nvlog {

template <typename T>
class ConcurrentQueue {
 private:
  mutable std::mutex mu_;
  std::queue<std::unique_ptr<T>> queue_;
  std::condition_variable cond_;

  ConcurrentQueue(const ConcurrentQueue&) = delete;
  ConcurrentQueue& operator=(const ConcurrentQueue&) = delete;

 public:
  ConcurrentQueue() = default;
  ~ConcurrentQueue() = default;

  bool TryDequeue(T& value) {
    std::unique_lock<std::mutex> lock(mu_);
    if (queue_.empty()) {
      return false;
    }
    value = std::move(*queue_.front());
    queue_.pop();
    return true;
  }

  void WaitAndDequeue(T& value) {
    std::unique_lock<std::mutex> lock(mu_);
    cond_.wait(lock, [this] { return !queue_.empty(); });
    value = std::move(*queue_.front());
    queue_.pop();
  }

  bool WaitAndDequeue(T& value, std::chrono::seconds timeout) {
    std::unique_lock<std::mutex> lock(mu_);
    if (!cond_.wait_for(lock, timeout, [this] { return !queue_.empty(); })) {
      return false;  // Timed out
    }
    value = std::move(*queue_.front());
    queue_.pop();
    return true;
  }

  void Enqueue(T value) {
    {
      std::lock_guard<std::mutex> lock(mu_);
      queue_.emplace(std::make_unique<T>(std::move(value)));
    }
    cond_.notify_all();
  }

  void Clear() {
    std::lock_guard<std::mutex> lock(mu_);
    std::queue<std::unique_ptr<T>> empty;
    std::swap(queue_, empty);
  }

  bool Empty() const {
    std::lock_guard<std::mutex> lock(mu_);
    return queue_.empty();
  }

  size_t Size() const {
    std::lock_guard<std::mutex> lock(mu_);
    return queue_.size();
  }
};

}  // namespace nvlog
