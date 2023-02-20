#include "stop_watch.hpp"

#include <thread>

int main() {
  StopWatch w;
  std::this_thread::sleep_for(std::chrono::seconds(1));
  w.print(1);
  return 0;
}

/*
$ g++ -o {stop_watch_test_cpp,.cpp} -Wall
$ ./stop_watch_test_cpp
*/