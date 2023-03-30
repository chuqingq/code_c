#include "queue.hpp"

#include <iostream>
#include <thread>

int main() {
  Queue<int> q(5);

  std::thread th([&]() {
    // 确保队列为空并且队列关闭时才退出
    while (!q.empty() || !q.closed()) {
      sleep(1);
      auto v = q.pull_front();
      std::cout << "recv " << v << std::endl;
    }
  });

  for (int i = 0; i < 10; i++) {
    q.push_back(i);
  }
  q.close();

  th.join();
  return 0;
}

// $ g++ -o queue_test queue_test.cpp -std=c++11 -Wall -g -lboost_thread
// ./queue_test
// recv 0
// recv 1
// recv 2
// recv 3
// recv 4
// recv 5
// recv 6
// recv 7
// recv 8
// recv 9