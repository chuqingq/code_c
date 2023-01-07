#include "service_discover.hpp"

#include <uv.h>

#include <chrono>
#include <iostream>
#include <thread>

int main() {
  bool received = false;
  auto loop = uv_default_loop();
  ServiceDiscover discover(loop);
  // TODO 是否要搞成单例
  // TODO 用例搞成跨进程、跨节点

  // 接收
  auto callback = [&](const std::string &topic, const uv_buf_t &value) {
    std::cout << "recv: " << topic << ": " << value.len << std::endl;
    received = true;
    discover.Stop();
  };
  discover.Recv("topic", callback);

  std::thread thread2([&] {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    // 发送
    char data[] = "hello";
    auto buf = uv_buf_init(data, sizeof(data));
    discover.Send("topic", buf);
  });
  uv_run(loop, UV_RUN_DEFAULT);

  if (!received) {
    std::cout << "assert error\n";
    return -1;
  }

  return 0;
}

/*
$ g++ -o service_discover_test{,.cpp} -g -Wall -luv
*/