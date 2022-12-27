// g++ test_async.cpp -std=c++14

#include <future>
#include <iostream>
#include <thread>


using namespace std::literals;

void f() {
  std::cout << "start f(): " << std::this_thread::get_id() << std::endl;
  std::this_thread::sleep_for(1s);
  std::cout << "stop" << std::endl;
}

int main() {
  std::cout << "main: " << std::this_thread::get_id() << std::endl;
  std::this_thread::sleep_for(100ms);

  auto fut = std::async(f);

  if (fut.wait_for(0ms) == std::future_status::deferred) {
    std::cout << "f被推迟执行" << std::endl;
    return -1;
  }

  while (fut.wait_for(100ms) != std::future_status::ready) {
    std::cout << "!=" << std::endl;
  }

  std::cout << "end" << std::endl;
  return 0;
}
// 没有大压力时会过一会儿就结束；有大压力时可能返回 std::future_status::deferred
/*
chuqq@cqqmbpr /V/w/t/c/c/20181002_async> g++ test_async.cpp -std=c++14
chuqq@cqqmbpr /V/w/t/c/c/20181002_async> ./a.out
main: 0x10e1df5c0
start f(): 0x70000e436000
!=
!=
!=
!=
!=
!=
!=
!=
!=
stop
end
*/