#pragma once

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

class PerfCount {
public:
  PerfCount() : loops_(0), stop_(false) {
    start_ = std::chrono::steady_clock::now();
    std::thread t([this]() {
      while (!stop_) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        this->print();
        loops_.store(0);
      }
    });
    thread_ = std::move(t);
  }

  void stop() {
    if (!stop_) {

      stop_ = true;
      thread_.join();
    }
  }

  ~PerfCount() { stop(); }

  void increase(uint64_t loops = 1) { loops_.fetch_add(loops); }

private:
  void print() {
    auto stop = std::chrono::steady_clock::now();
    auto ns =
        std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start_)
            .count();
    std::cout << "PerfCount: total " << ns << " ns; average " << ns / loops_
              << " ns/loop.\n";
    start_ = stop;
    // start_ = std::chrono::steady_clock::now();
  }

  std::chrono::time_point<std::chrono::steady_clock> start_;
  std::atomic<uint64_t> loops_;
  bool stop_;
  std::thread thread_;
};
