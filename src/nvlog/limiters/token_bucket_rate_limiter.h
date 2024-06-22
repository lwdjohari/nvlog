#pragma once
#include <atomic>
#include <chrono>
#include <thread>
#include <mutex>

#include "nvlog/limiters/rate_limiter.h"

namespace nvlog {
namespace limiters {
class TokenBucketRateLimiter : public RateLimiter {
 public:
  TokenBucketRateLimiter(size_t max_tokens,
                         std::chrono::milliseconds refill_interval,
                         size_t refill_amount);
  bool Allow() override;

 private:
  void Refill();

  size_t max_tokens_;
  std::chrono::milliseconds refill_interval_;
  size_t refill_amount_;
  std::atomic<size_t> tokens_;
  std::chrono::steady_clock::time_point last_refill_;
  std::mutex mutex_;
};

}  // namespace limiters

}  // namespace nvlog
