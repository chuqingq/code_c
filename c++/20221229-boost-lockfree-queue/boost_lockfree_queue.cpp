#include <stdio.h>

#include <boost/lockfree/queue.hpp>

struct Node {
  void *p;
  int ref_count;
  void *p2;
  void *p3;
};

int main() {
  boost::lockfree::queue<Node> q(10);
  printf("lockfree: %d\n", q.is_lock_free());
  return 0;
}

/*
$ g++ -o boost_lockfree_queue boost_lockfree_queue.cpp
$ ./boost_lockfree_queue
lockfree: 1
*/