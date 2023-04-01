#pragma once

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

class PerfStats {
public:
  PerfStats(const char *name = "unknown") : name_(name), loops_(0) {
    start_ = std::chrono::steady_clock::now();
    std::thread t([this]() {
      while (1) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        this->Print();
      }
    });
    thread_ = std::move(t);
  }

  void Stop() {
    if (thread_.joinable()) {
      pthread_cancel(thread_.native_handle());
      Print();
      thread_.join();
    }
  }

  ~PerfStats() { Stop(); }

  void Incr(uint64_t loops = 1) { loops_.fetch_add(loops); }

private:
  void Print() {
    auto loops = loops_.load();
    loops_.fetch_add(-loops);

    auto stop = std::chrono::steady_clock::now();
    auto ns =
        std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start_)
            .count();
    std::cout << "PerfCount[" << std::string(name_) << "]: total " << ns
              << " ns; loops: " << loops << "; average " << ns / loops
              << " ns/loop.\n";
    start_ = stop;
    // start_ = std::chrono::steady_clock::now();
  }

  const char *name_;
  std::chrono::time_point<std::chrono::steady_clock> start_;
  std::atomic<uint64_t> loops_;
  std::thread thread_;
};
