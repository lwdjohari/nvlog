#include "nvlog/logger.h"

namespace nvlog {

// Define the static member variables
std::shared_ptr<Logger> Logger::instance_ = nullptr;
std::mutex Logger::mutex_;
}  // namespace nvlog
