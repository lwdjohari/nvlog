#include <thread>
#include <vector>

#include "nvlog/nvlog.h"

static std::vector<uint64_t> products_;
static uint64_t worker_number_;
std::vector<std::thread> producers;
void LogProducer(int thread_id, int log_count) {
  for (int i = 0; i < log_count; ++i) {
    LOG_INFO_T("Log message from worker #" + std::to_string(thread_id) +
                   " message #" + std::to_string(i),
               "W#" + std::to_string(thread_id))
    products_[thread_id] = products_[thread_id] + 1;
  }
}

void CreateProducers(int num_producers, int logs_per_producer) {
  // Create and start producer threads

  for (int i = 0; i < num_producers; ++i) {
    producers.emplace_back(LogProducer, i, logs_per_producer);
  }

  // Join producer threads
  for (auto& producer : producers) {
    if (producer.joinable()) {
      producer.join();
    }
  }
}

int main(int argc, char* argv[]) {
  // Set number of producer threads and number of logs per thread
  int num_producers = 4;        // Default number of producers
  int logs_per_producer = 100;  // Default number of logs per producer

  try {
    if (argc > 1) {
      num_producers = std::stoi(argv[1]);
    }
    if (argc > 2) {
      logs_per_producer = std::stoi(argv[2]);
    }
  } catch (const std::exception& e) {
    std::cerr << "Invalid argument: " << e.what() << std::endl;
    return 1;
  }

  worker_number_ = num_producers;
  products_ = std::vector<uint64_t>();
  for (size_t i = 0; i < worker_number_; i++) {
    products_.push_back(0);
  }

  // Initialize the logger and sinks
  auto console_sink = std::make_shared<nvlog::ConsoleSink>();
  console_sink->SetFormatter(nvlog::SimpleFormatter);

  auto file_sink = std::make_shared<nvlog::DeferredFileSink>(
      "app.log", 8 * 1024, std::chrono::seconds(10));

  std::vector<std::shared_ptr<nvlog::Sink>> sinks = {console_sink, file_sink};

  nvlog::Logger::RegisterLogger(sinks);
  nvlog::Logger::Get()->StartEngine();

  // cppcheck-suppress unknownMacro
  LOG_TRACE_T("Logger Initialize", "LOGGER")
  LOG_TRACE_COND_T(num_producers == 4, "Producer is 4", "COND")
  CreateProducers(num_producers, logs_per_producer);
  LOG_TRACE_T("Logger shutdown", "LOGGER")
  nvlog::Logger::Get()->ShutdownEngine();

  for (auto& p : products_) {
    std::cout << "product:" << p << std::endl;
  }

  return 0;
}