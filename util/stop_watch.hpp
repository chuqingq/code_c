#pragma once

#include <iostream>
#include <chrono>

class StopWatch
{
public:
    StopWatch()
    {
        start();
    }

    void start()
    {
        start_ = std::chrono::steady_clock::now();
    }

    void print(uint64_t loops)
    {
        auto stop = std::chrono::steady_clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                      stop - start_)
                      .count();
        std::cout << "StopWatch: total " << ns << " ns; average " << ns / loops << " ns/loop.\n";
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> start_;
};
