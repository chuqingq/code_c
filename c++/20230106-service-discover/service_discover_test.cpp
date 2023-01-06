#include "service_discover.hpp"

#include <uv.h>

#include <iostream>
#include <thread>

int main() {
  bool received = false;
  auto loop = uv_default_loop();
  ServiceDiscover discover(loop);
  // TODO 是否要搞成单例
  // TODO 用例搞成跨进程、跨节点

  // 接收
  auto callback = [&](const std::string &topic, const std::string &key,
                      const Buffer &value) {
    std::cout << "recv: " << topic << ": " << key << ": " << value.size_
              << std::endl;
    received = true;
    discover.Stop();
  };
  discover.Recv("topic", callback);

  // 发送
  char data[] = "hello";
  Buffer buf{data, sizeof(data)};
  discover.Send("topic", "key", buf);

  uv_run(loop, UV_RUN_DEFAULT);

  if (!received) {
    std::cout << "assert error\n";
    return -1;
  }

  return 0;
}

/*
$ g++ -o service_discover_test{,.cpp} -Wall
*/