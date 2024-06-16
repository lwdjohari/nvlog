#include "nvlog/limiters/token_bucket_rate_limiter.h"

namespace nvlog {
namespace limiters {
TokenBucketRateLimiter::TokenBucketRateLimiter(
    size_t max_tokens, std::chrono::milliseconds refill_interval,
    size_t refill_amount)
                : max_tokens_(max_tokens),
                  refill_interval_(refill_interval),
                  refill_amount_(refill_amount),
                  tokens_(max_tokens),
                  last_refill_(std::chrono::steady_clock::now()) {}

bool TokenBucketRateLimiter::Allow() {
  std::lock_guard<std::mutex> lock(mutex_);
  Refill();
  if (tokens_ > 0) {
    --tokens_;
    return true;
  }
  return false;
}

void TokenBucketRateLimiter::Refill() {
  auto now = std::chrono::steady_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(now - last_refill_);
  if (duration >= refill_interval_) {
    size_t new_tokens = (duration / refill_interval_) * refill_amount_;
    tokens_ = std::min(tokens_ + new_tokens, max_tokens_);
    last_refill_ = now;
  }
}
}  // namespace limiters

}  // namespace nvlog
