#include "defer.hpp"

#include <iostream>

int main() {
  int a = 123;
  DEFER([a]() { std::cout << "defer: " << a << std::endl; });
  std::cout << "main\n";
  return 0;
}
/*
$ g++ defer_test.cpp -o defer_test
$ ./defer_test
main
defer: 123
*/