#include <chrono>
#include <iostream>
#include <thread>

#include "bounded_blocking_queue.hpp"

int main() {
  const int loops = 1e5;
  BoundedBlockingQueue<int> q1(1), q2(1);

  auto startTime = std::chrono::steady_clock::now();

  std::thread ping([&] {
    int v;
    for (int i = 0; i < loops; i++) {
      q1.push_back(i);
      if (!q2.pop_front(v) || v != i) {
        std::cout << "ping error: " << i << "\n";
      }
    }
  });

  std::thread pong([&] {
    int v;
    for (int i = 0; i < loops; i++) {
      if (!q1.pop_front(v) || v != i) {
        std::cout << "pong error: " << i << "\n";
      }
      q2.push_back(i);
    }
  });

  ping.join();
  pong.join();

  auto endTime = std::chrono::steady_clock::now();

  auto duration =
      std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime)
          .count();
  auto durationPer = duration / loops;

  std::cout << "elapsed: " << duration / 1000 << " us, "
            << "per loop: " << durationPer << " ns/loop.\n";

  return 0;
}
/*
$ g++ -o bounded_blocking_queue_cpp_test -Wall \
bounded_blocking_queue_cpp_test.cpp

$ ./bounded_blocking_queue_cpp_test
// matebook13@home
elapsed: 5267101 us, per loop: 52671 ns/loop.

valgrind : All heap blocks were freed-- no leaks are possible
*/