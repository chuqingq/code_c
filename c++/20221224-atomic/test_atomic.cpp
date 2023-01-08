#include <atomic>
#include <iostream>
#include <memory>
#include <utility>

struct A {
  int a[100];
};
struct B {
  int x;
  int y;
};
struct C {
  int x;
  void *y;
};
int main() {
  std::cout << std::boolalpha << "std::atomic<A> is lock free? "
            << std::atomic<A>{}.is_lock_free() << '\n'
            << "std::atomic<B> is lock free? "
            << std::atomic<B>{}.is_lock_free() << '\n'
            << "std::atomic<C> is lock free? "
            << std::atomic<C>{}.is_lock_free() << '\n';
}

/*
$ g++ -o test_atomic test_atomic.cpp -std=c++20 -g -Wall -latomic
$ ./test_atomic
std::atomic<A> is lock free? false
std::atomic<B> is lock free? true
std::atomic<C> is lock free? false
std::atomic<std::shared_ptr<int>> is lock free? false
*/
