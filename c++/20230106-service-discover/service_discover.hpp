#pragma once

#include <uv.h>

#include <functional>
#include <map>
#include <string>
#include <vector>

struct Buffer {
  char *data_;
  size_t size_;

  Buffer() : data_(nullptr), size_(0) {}
  Buffer(char *data, size_t size) : data_(data), size_(size) {}
};

class ServiceDiscover {
 private:
  uv_udp_t udp_;
  using RecvCallback = std::function<void(
      const std::string &topic, const std::string &key, const Buffer &value)>;
  std::map<std::string, std::vector<RecvCallback>> callbacks_;

  inline int Send(const Buffer &b);

 public:
  // constructor
  inline ServiceDiscover(uv_loop_t *loop);

  inline int Send(const std::string &topic, const std::string &key,
                  const Buffer &value);
  inline int Recv(const std::string &topic, RecvCallback callback);
  inline int Stop();
};

ServiceDiscover::ServiceDiscover(uv_loop_t *loop) {
  // TODO 创建udp组播，准备接收消息
}

int ServiceDiscover::Send(const Buffer &b) {
  // TODO
}

int ServiceDiscover::Send(const std::string &topic, const std::string &key,
                          const Buffer &value) {
  // TODO
}

int ServiceDiscover::Recv(const std::string &topic, RecvCallback callback) {
  // TODO
}

int ServiceDiscover::Stop() {
  // TODO
}