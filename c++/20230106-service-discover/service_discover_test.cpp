#include "service_discover.hpp"

#include <string.h>
#include <uv.h>

#include <chrono>
#include <iostream>
#include <thread>

int main() {
  bool received = false;
  const std::string kTopic = "topic111";
  const char data[] = "hello";
  auto loop = uv_default_loop();
  ServiceDiscover discover(loop);

  // 接收
  auto callback = [&](const std::string &topic, const uv_buf_t &value) {
    std::cout << "recv topic: " << topic << ": " << value.base << std::endl;
    assert(0 == memcmp(data, value.base, sizeof(data)));
    received = true;
    discover.Stop();
  };
  discover.Recv(kTopic, callback);

  // 发送
  ServiceDiscover discover2(loop);
  auto buf = uv_buf_init((char *)data, sizeof(data));
  discover2.Send(kTopic, buf);
  discover2.Stop();

  uv_run(loop, UV_RUN_DEFAULT);

  assert(received);
  return 0;
}

/*
$ g++ -o service_discover_test{,.cpp} -g -Wall -luv
*/