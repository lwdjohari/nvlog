#pragma once

namespace nvlog {
namespace limiters {

class RateLimiter {
 public:
  virtual ~RateLimiter() = default;

  // Checks if the current operation is allowed under the rate limit
  virtual bool Allow() = 0;
};

class NullLimiter : public RateLimiter {
public:
    bool Allow() override {
        return true;
    }
};

}  // namespace limiters
}  // namespace nvlog
