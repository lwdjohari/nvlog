#pragma once

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

#include "nvlog/declare.h"
#include "nvlog/formatter.h"
#include "nvlog/sink.h"

namespace nvlog {

class ConsoleSink : public AsyncSink {
 private:
  /* data */
 public:
  ConsoleSink()

  {}

  ~ConsoleSink() {
    // if(IsRun()){
    //   Shutdown(false);
    // }
  }

 protected:
  void Process(const std::shared_ptr<LogMessage>& log_message) override {
    std::ostringstream ss;
    if (formatter_) {
      formatter_(ss, *log_message);
    } else {
      DefaultFormatter(ss, *log_message);
    }
    PrintToConsole(ss.str());
  }

 private:
  std::mutex mtx_;
  void PrintToConsole(const std::string& msg) {
    std::lock_guard<std::mutex> lock(mtx_);
    std::cout << msg << std::endl;
  }
};

}  // namespace nvlog
