#include "bounded_blocking_queue.h"

#include <chrono>
#include <iostream>
#include <thread>

int main() {
  const int loops = 1000000; // 10000000;
  queue_t q1, q2;
  queue_init(&q1, 2);
  queue_init(&q2, 2);
  int data;

  auto startTime = std::chrono::steady_clock::now();

  std::thread ping([&] {
    for (int i = 0; i < loops; i++) {
      data = i;
      queue_put(&q1, &data);
      int *d = (int *)queue_get(&q2);
      if (*d != i) {
        std::cout << "ping error: " << i << "\n";
      }
    }
  });

  std::thread pong([&] {
    for (int i = 0; i < loops; i++) {
      int *d = (int *)queue_get(&q1);
      if (*d != i) {
        std::cout << "pong error: " << i << "\n";
      }
      data = i;
      queue_put(&q2, &data);
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

  queue_destroy(&q1);
  queue_destroy(&q2);

  return 0;
}
/*
$ g++ -o bounded_blocking_queue -Wall bounded_blocking_queue_test.cpp
$ ./bounded_blocking_queue
elapsed: 28459393 us, per loop: 28459 ns/loop.

valgrind: All heap blocks were freed -- no leaks are possible
*/