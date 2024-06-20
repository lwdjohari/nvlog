# NvLog

<img src="nvlog.png" width="300"/>

Modern C++ Logger with multithread support, custom sink and custom formatter. No more latency added just waiting for log to be done. Targeted for C++14 &amp; C++17.

> [!WARNING]
> Currently compatibility with C++14 is not yet throughly tested.<br/>
> Status : WIP, Experimental & Unstable.

## Feature
- Multithreaded (Log will send to channel immediately)
- Channel Multi-worker support (default to 1 worker)
- Console Sink 
- Defered File Sink
- Custom Sink Support
- Easy custom formatter for each sink

> [!Warning]
> NvLog is never intended be designed as non multithreaded logger 

## Logger Initialization

### Initialize logger

```cpp

#include <iostream>
#include "nvlog/nvlog.h"

int main(){
 // Initialize the logger and sinks
  auto console_sink = std::make_shared<nvlog::ConsoleSink>();

  auto file_sink = std::make_shared<nvlog::DeferredFileSink>(
      "app.log", 8 * 1024, std::chrono::seconds(10));

  std::vector<std::shared_ptr<nvlog::Sink>> sinks = {console_sink, file_sink};

  nvlog::Logger::RegisterLogger(sinks);
  nvlog::Logger::Get()->StartEngine();

  // Server loop blocking main thread from exit
  ServerLoop();

  // close the engine
  // engine designed to implemented RAII
  // auto cleanup even the shutdown isn't called
  nvlog::Logger::Get()->ShutdownEngine();

  return 0;
}

```

## Using The Logger

NvLog use macro to wrap all info need to build the logger message.

### Logging With Tag
```cpp
LOG_TRACE_T("MSG", "TAG")
LOG_DEBUG_T("MSG", "TAG")
LOG_WARN_T("MSG", "TAG")
LOG_ERROR_T("MSG", "TAG")
LOG_FATAL_T("MSG", "TAG")

```

### Logging Without Tag

```cpp
LOG_TRACE("MSG")
LOG_DEBUG("MSG")
LOG_WARN("MSG")
LOG_ERROR("MSG")
LOG_FATAL("MSG")
```

### Conditional Logging
Example
```cpp
int num_producer = 4;
LOG_TRACE_COND_T(num_producers == 4, "Producer is 4", "COND")
```

Available Macro
```cpp
LOG_TRACE_COND_T(CONDITION, "MSG", "TAG")
LOG_DEBUG_COND_T(CONDITION, "MSG", "TAG")
LOG_WARN_COND_T(CONDITION, "MSG", "TAG")
LOG_ERROR_COND_T(CONDITION, "MSG", "TAG")
LOG_FATAL_COND_T(CONDITION, "MSG", "TAG")
```

```cpp
LOG_TRACE_COND(CONDITION, "MSG")
LOG_DEBUG_COND(CONDITION, "MSG")
LOG_WARN_COND(CONDITION, "MSG")
LOG_ERROR_COND(CONDITION, "MSG")
LOG_FATAL_COND(CONDITION, "MSG")
```

## Custom Formatter
For each sink, you can customize based on default formatter callback.

### Custom Formatter Example

MyFormatter Example

```cpp

void MyFormatter(std::ostringstream& buffer, const LogMessage& log_message) {
  auto time = log_message.timestamp;
  auto time_t = std::chrono::system_clock::to_time_t(time);
  auto tm = *std::localtime(&time_t);
  auto ms = std::chrono::duration_cast<std::chrono::microseconds>(
                time.time_since_epoch()) %
            1000000;

  // clang-format off
  buffer <<"[";
  nvlog::DefaultLevelFormatter(buffer, log_message.log_level);
  buffer 
      << "] " 
      << std::setw(4) << std::setfill('0') << (1900 + tm.tm_year) << "-"
      << std::setw(2) << std::setfill('0') << (1 + tm.tm_mon) << "-"
      << std::setw(2) << tm.tm_mday
      << ' '
      << std::setw(2) << tm.tm_hour << ':'
      << std::setw(2) << tm.tm_min << ':'
      << std::setw(2) << tm.tm_sec << '.'
      << std::setw(6) << ms.count() << " "
      << "[" << log_message.tag << "] " << log_message.message;
  // clang-format on

  buffer << std::resetiosflags(std::ios::adjustfield | std::ios::basefield |
                               std::ios::floatfield)
         << std::setfill('0');
}

```

Register the ```MyFormatter``` to the sink.

```cpp

int main(){
  auto console_sink = std::make_shared<nvlog::ConsoleSink>();
  console_sink->SetFormatter(MyFormatter);

  // ...the rest implementations...
}

```

### Default Formatter Implementation 

```cpp

void DefaultFormatter(std::ostringstream& buffer, const LogMessage& log_message) {
  auto time = log_message.timestamp;
  auto time_t = std::chrono::system_clock::to_time_t(time);
  auto tm = *std::localtime(&time_t);
  auto ms = std::chrono::duration_cast<std::chrono::microseconds>(
                time.time_since_epoch()) %
            1000000;

  // clang-format off
  DefaultLevelFormatter(buffer, log_message.log_level);
  buffer 
      << std::setw(4) << std::setfill('0') << (1900 + tm.tm_year) << "-"
      << std::setw(2) << std::setfill('0') << (1 + tm.tm_mon) << "-"
      << std::setw(2) << tm.tm_mday
      << ' '
      << std::setw(2) << tm.tm_hour << ':'
      << std::setw(2) << tm.tm_min << ':'
      << std::setw(2) << tm.tm_sec << '.'
      << std::setw(6) << ms.count()
      << ' '
      << std::setw(5) << std::setfill(' ') << log_message.thread_id << std::setfill('0')
      << ' '
      << log_message.file << ':' << log_message.line << "]\n" 
      << std::left << std::setw(8) << std::setfill(' ') << " " 
      << "[" << log_message.tag << "] " << log_message.message;
  // clang-format on

  buffer << std::resetiosflags(std::ios::adjustfield | std::ios::basefield |
                               std::ios::floatfield)
         << std::setfill('0');
}

```

## Utility Functions

NvLog has few utility helper to help with logger

```nvlog::GetThreadId()``` <br/>
Get current thread id.

```nvlog::DateTimeComponent::FromTimestamp( const std::chrono::system_clock::time_point& tp)```<br/>
Explode time_point to datepart.

```nvlog::DateTimeComponent```<br/>
Beside hold the datetime parts, the struct also has methods to help with formatting the dates and write it to ```std::ostringstream```.
## Contributions

Currently we are still on-going roadmap design and architectural design that might be lead to complete rewrite or complete breaking changes.
We might accept contributors when everything above have better & crytal-clears roadmap.

## License

MIT License

Copyright (c) 2024 Linggawasistha Djohari

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
