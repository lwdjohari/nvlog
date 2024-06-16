# NvLog
Modern C++ Logger with multithread support, custom sink and custom formatter. No more latency added just waiting for log to be done. Targeted for C++14 &amp; C++17.

> [!WARNING]
> Currently compatibility with C++14 is not yet throughly tested.<br/>
> Status : WIP, Experimental & Unstable.

## Feature
- Multithreaded (Log will send to channel immediately)
- Channel Multi-worker support (default to 1 worker)
- Console Sink 
- Defered File Sink
- Custom Sink Supoort
- Easy custom formatter for each sink

> [!Warning]
> NvLog is never intended be designed as non multithreaded logger 

## Custom Formatter
For each sink, you can customize based on default formatter callback.

Default Formatter 

```cpp

void DefaultFormatter(std::ostringstream& buffer,
                             const LogMessage& log_message) {
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
