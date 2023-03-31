#include "stop_watch.hpp"

#include <chrono>
#include <thread>
#include <unistd.h>

int main() {
  StopWatch watch;

  for (auto i = 0; i < 10; i ++) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    watch.AddLoops(1);
  }
  
  watch.StopAndStats();
  return 0;
}

/*
$ g++ -o stop_watch_test{,.cpp} -Wall -g
$ ./stop_watch_test
StopWatch: duration 1001040897 ns; loops: 10; average 100104089 ns/loop.
*/