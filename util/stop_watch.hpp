#pragma once

#include <chrono>
#include <iostream>

class StopWatch {
public:
  StopWatch() { Start(); }

  void Start() {
    start_ = std::chrono::steady_clock::now();
    loops_ = 0;
  }

  void AddLoops(uint64_t loops = 1) { loops_ += loops; }

  void StopAndStats() {
    auto stop = std::chrono::steady_clock::now();
    auto ns =
        std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start_)
            .count();
    std::cout << "StopWatch: duration " << ns << " ns; loops: " << loops_ << "; average " << ns / loops_
              << " ns/loop.\n";
  }

private:
  std::chrono::time_point<std::chrono::steady_clock> start_;
  uint64_t loops_;
};
