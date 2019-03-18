//
// Copyright(c) 2015 Gabi Melman.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//
//
// spdlog usage example
//
//

#include <cstdio>

void stdout_logger_example();
void basic_example();
void rotating_example();
void daily_example();
void async_example();
void binary_example();
void trace_example();
void multi_sink_example();
void user_defined_example();
void err_handler_example();
void syslog_example();
void clone_example();

// chuqq 让trace生效，才能打印出源码文件名、行号、函数名等
# define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#define FMT_STRING_ALIAS 1

#include "spdlog/spdlog.h"

int main(int, char *[])
{
    spdlog::info("这是中文 Welcome to spdlog version {}.{}.{} {} !", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);
    spdlog::warn("Easy padding in numbers like {:08d}", 12);
    spdlog::critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
    spdlog::info("Support for floats {:03.2f}", 1.23456);
    spdlog::info("Positional args are {1} {0}..", "too", "supported");
    spdlog::info("{:>8} aligned, {:<8} aligned", "right", "left");

    // Runtime log levels
    spdlog::set_level(spdlog::level::info); // Set global log level to info
    spdlog::debug("This message should not be displayed!");
    spdlog::set_level(spdlog::level::trace); // Set specific logger's log level
    spdlog::debug("This message should be displayed..");

    // Customize msg format for all loggers
    // spdlog::set_pattern("[%H:%M:%S %z] [%^%L%$] [thread %t] %v");
    spdlog::set_pattern("%Y-%m-%d %H:%M:%S.%e %l %@ %! : %v");
    spdlog::info("This an info message with custom format");
    // spdlog::set_pattern("%+"); // back to default format

    try
    {
        stdout_logger_example();
        basic_example();
        rotating_example();
        daily_example();
        clone_example();
        async_example();
        binary_example();
        multi_sink_example();
        user_defined_example();
        err_handler_example();
        trace_example();

        // Flush all *registered* loggers using a worker thread every 3 seconds.
        // note: registered loggers *must* be thread safe for this to work correctly!
        spdlog::flush_every(std::chrono::seconds(3));

        // Apply some function on all registered loggers
        spdlog::apply_all([&](std::shared_ptr<spdlog::logger> l) { l->info("End of example."); });

        // Release all spdlog resources, and drop all loggers in the registry.
        // This is optional (only mandatory if using windows + async log).
        spdlog::shutdown();
    }

    // Exceptions will only be thrown upon failed logger or sink construction (not during logging).
    catch (const spdlog::spdlog_ex &ex)
    {
        std::printf("Log initialization failed: %s\n", ex.what());
        return 1;
    }
}

#include "spdlog/sinks/stdout_color_sinks.h"
// or #include "spdlog/sinks/stdout_sinks.h" if no colors needed.
void stdout_logger_example()
{
    // Create color multi threaded logger.
    auto console = spdlog::stdout_color_mt("console");
    // or for stderr:
    // auto console = spdlog::stderr_color_mt("error-logger");
}

#include "spdlog/sinks/basic_file_sink.h"
void basic_example()
{
    // Create basic file logger (not rotated).
    auto my_logger = spdlog::basic_logger_mt("file_logger", "logs/basic-log.txt");
}

#include "spdlog/sinks/rotating_file_sink.h"
void rotating_example()
{
    // Create a file rotating logger with 5mb size max and 3 rotated files.
    auto rotating_logger = spdlog::rotating_logger_mt("some_logger_name", "logs/rotating.txt", 1048576 * 5, 3);
}

#include "spdlog/sinks/daily_file_sink.h"
void daily_example()
{
    // Create a daily logger - a new file is created every day on 2:30am.
    auto daily_logger = spdlog::daily_logger_mt("daily_logger", "logs/daily.txt", 2, 30);
}

// Clone a logger and give it new name.
// Useful for creating component/subsystem loggers from some "root" logger.
void clone_example()
{
    auto network_logger = spdlog::default_logger()->clone("network");
    network_logger->info("Logging network stuff..");
}

#include "spdlog/async.h"
void async_example()
{
    // Default thread pool settings can be modified *before* creating the async logger:
    // spdlog::init_thread_pool(32768, 1); // queue with max 32k items 1 backing thread.
    auto async_file = spdlog::basic_logger_mt<spdlog::async_factory>("async_file_logger", "logs/async_log.txt");
    // alternatively:
    // auto async_file = spdlog::create_async<spdlog::sinks::basic_file_sink_mt>("async_file_logger", "logs/async_log.txt");

    for (int i = 1; i < 101; ++i)
    {
        async_file->info("Async message #{}", i);
    }
}

// Log binary data as hex.
// Many types of std::container<char> types can be used.
// Iterator ranges are supported too.
// Format flags:
// {:X} - print in uppercase.
// {:s} - don't separate each byte with space.
// {:p} - don't print the position on each line start.
// {:n} - don't split the output to lines.

#include "spdlog/fmt/bin_to_hex.h"
void binary_example()
{
    std::vector<char> buf;
    for (int i = 0; i < 80; i++)
    {
        buf.push_back(static_cast<char>(i & 0xff));
    }
    spdlog::info("Binary example: {}", spdlog::to_hex(buf));
    spdlog::info("Another binary example:{:n}", spdlog::to_hex(std::begin(buf), std::begin(buf) + 10));
    // more examples:
    // logger->info("uppercase: {:X}", spdlog::to_hex(buf));
    // logger->info("uppercase, no delimiters: {:Xs}", spdlog::to_hex(buf));
    // logger->info("uppercase, no delimiters, no position info: {:Xsp}", spdlog::to_hex(buf));
}

// Compile time log levels.
// define SPDLOG_ACTIVE_LEVEL to required level (e.g. SPDLOG_LEVEL_TRACE)
void trace_example()
{
    // trace from default logger
    SPDLOG_TRACE("Some trace message.. {} ,{}", 1, 3.23);
    // debug from default logger
    SPDLOG_DEBUG("Some debug message.. {} ,{}", 1, 3.23);

    // trace from logger object
    auto logger = spdlog::get("file_logger");
    SPDLOG_LOGGER_TRACE(logger, "another trace message");
}

// A logger with multiple sinks (stdout and file) - each with a different format and log level.
void multi_sink_example()
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::warn);
    console_sink->set_pattern("[multi_sink_example] [%^%l%$] %v");

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/multisink.txt", true);
    file_sink->set_level(spdlog::level::trace);

    spdlog::logger logger("multi_sink", {console_sink, file_sink});
    logger.set_level(spdlog::level::debug);
    logger.warn("this should appear in both console and file");
    logger.info("this message should not appear in the console, only in the file");
}

// User defined types logging by implementing operator<<
#include "spdlog/fmt/ostr.h" // must be included
struct my_type
{
    int i;
    template<typename OStream>
    friend OStream &operator<<(OStream &os, const my_type &c)
    {
        return os << "[my_type i=" << c.i << "]";
    }
};

void user_defined_example()
{
    spdlog::info("user defined type: {}", my_type{14});
}

// Custom error handler. Will be triggered on log failure.
void err_handler_example()
{
    // can be set globally or per logger(logger->set_error_handler(..))
    spdlog::set_error_handler([](const std::string &msg) { printf("*** Custom log error handler: %s ***\n", msg.c_str()); });
}

// syslog example (linux/osx/freebsd)
#ifndef _WIN32
#include "spdlog/sinks/syslog_sink.h"
void syslog_example()
{
    std::string ident = "spdlog-example";
    auto syslog_logger = spdlog::syslog_logger_mt("syslog", ident, LOG_PID);
    syslog_logger->warn("This is warning that will end up in syslog.");
}
#endif

// Android example.
#if defined(__ANDROID__)
#include "spdlog/sinks/android_sink.h"
void android_example()
{
    std::string tag = "spdlog-android";
    auto android_logger = spdlog::android_logger_mt("android", tag);
    android_logger->critical("Use \"adb shell logcat\" to view this message.");
}

#endif

/*
- spdlog分析
  - 纯头文件
  - 快
  - 格式化：使用fmt库
  - 支持快速异步模式？
  - DONE 支持定制格式？
  - 支持多线程？
    auto logger = spdlog::basic_logger_mt(...);
    set_default_logger...
  - 支持多种目标？旋转、每日、控制台、syslog、win debugger、
    auto file_logger = spd::rotating_logger_mt("file_logger", "logs/mylogfile", 1048576 * 5, 3); // 可以设置名称、路径、大小
    spdlog::set_default_logger(file_logger);
  - 严重级别过滤：支持运行时和编译时？
    logger.set_level(spdlog::level::debug);
  - 支持二进制日志？
  - 自己的问题
    - 参数数量如何在编译时就校验？[*** LOG ERROR ***] [2019-03-17 17:01:01] [] argument index out of range
    - 能否同时支持%s和{}？

> ./example-debug
[2019-03-17 15:01:01.149] [info] Welcome to spdlog version 1.3.1 !
[2019-03-17 15:01:01.150] [warning] Easy padding in numbers like 00000012
[2019-03-17 15:01:01.150] [critical] Support for int: 42;  hex: 2a;  oct: 52; bin: 101010
[2019-03-17 15:01:01.150] [info] Support for floats 1.23
[2019-03-17 15:01:01.150] [info] Positional args are supported too..
[2019-03-17 15:01:01.150] [info]    right aligned, left     aligned
[2019-03-17 15:01:01.150] [debug] This message should be displayed..
2019-03-17 15:01:01.150 info   : This an info message with custom format
2019-03-17 15:01:01.150 info   : Logging network stuff..
2019-03-17 15:01:01.153 info   : Binary example:
0000: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f
0020: 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f
0040: 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f
2019-03-17 15:01:01.153 info   : Another binary example: 00 01 02 03 04 05 06 07 08 09
[multi_sink_example] [warning] this should appear in both console and file
2019-03-17 15:01:01.154 info   : user defined type: [my_type i=14]
2019-03-17 15:01:01.154 trace example.cpp:169 trace_example : Some trace message.. 1 ,3.23
2019-03-17 15:01:01.154 debug example.cpp:171 trace_example : Some debug message.. 1 ,3.23
2019-03-17 15:01:01.154 info   : End of example.
2019-03-17 15:01:01.154 info   : End of example.

*/
