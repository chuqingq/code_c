#pragma once

#include <uv.h>

#include <cassert>
#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <vector>

class ServiceDiscover {
 public:
  // constructors/destructors
  inline ServiceDiscover(uv_loop_t *loop);

  inline int Send(const std::string &topic, const uv_buf_t &value);
  using RecvCallback =
      std::function<void(const std::string &topic, const uv_buf_t &value)>;
  inline void Recv(const std::string &topic, RecvCallback callback);
  inline void Stop();

 private:
  const char *kGroupcastIp = "239.255.0.1";
  const uint16_t kGroupcastPort = 8888;

  struct sockaddr_in groupcastaddr_;
  uv_udp_t udp_server_;
  uv_udp_t udp_client_;
  char buffer_[1500];

  std::map<std::string, std::vector<RecvCallback>> callbacks_;
  std::mutex mutex_;

  friend inline void buf_alloc(uv_handle_t *tcp, size_t size, uv_buf_t *buf);
  friend inline void pinger_read_cb(uv_udp_t *udp, ssize_t nread,
                                    const uv_buf_t *buf,
                                    const struct sockaddr *addr,
                                    unsigned flags);
};

inline void buf_alloc(uv_handle_t *tcp, size_t size, uv_buf_t *buf) {
  auto discover = (ServiceDiscover *)tcp->data;
  buf->base = discover->buffer_;
  buf->len = sizeof(discover->buffer_);
}

inline void pinger_read_cb(uv_udp_t *udp, ssize_t nread, const uv_buf_t *buf,
                           const struct sockaddr *addr, unsigned flags) {
  if (nread == 0) {
    return;
  } else if (nread < 0) {
    perror("pinger_read_cb nread < 0");
    return;
  }
  ServiceDiscover *discover = (ServiceDiscover *)udp->data;

  if (nread > 0) {
    // printf("read_cb: %ld %s\n", nread, buf->base);
    std::string topic(buf->base);
    auto b =
        uv_buf_init(buf->base + topic.size() + 1, buf->len - topic.size() - 1);
    std::lock_guard<std::mutex> lg(discover->mutex_);
    for (auto cb : discover->callbacks_[topic]) {
      cb(topic, b);
    }
  }
}

ServiceDiscover::ServiceDiscover(uv_loop_t *loop) {
  int r = uv_udp_init(loop, &udp_server_);
  assert(r == 0);
  udp_server_.data = this;
  r = uv_udp_init(loop, &udp_client_);
  assert(r == 0);
  udp_client_.data = this;

  struct sockaddr_in addr;
  uv_ip4_addr("0.0.0.0", kGroupcastPort, &addr);

  r = uv_ip4_addr(kGroupcastIp, kGroupcastPort, &groupcastaddr_);
  assert(r == 0);

  r = uv_udp_bind(&udp_server_, (struct sockaddr *)&addr, UV_UDP_REUSEADDR);
  if (r < 0) {
    perror("uv_udp_bind error");
    throw;
  }

  r = uv_udp_set_membership(&udp_server_, kGroupcastIp, NULL, UV_JOIN_GROUP);
  if (r < 0) {
    perror("uv_udp_set_membership error");
    throw;
  }

  r = uv_udp_recv_start(&udp_server_, buf_alloc, pinger_read_cb);
  if (r < 0) {
    perror("uv_udp_recv_start error");
    throw;
  }
}

int ServiceDiscover::Send(const std::string &topic, const uv_buf_t &value) {
  std::vector<char> b;
  b.resize(value.len + topic.size() + 1);

  std::copy(topic.begin(), topic.end(), b.begin());
  b[topic.size()] = '\0';
  std::copy(value.base, value.base + value.len, b.begin() + topic.size() + 1);

  auto buf = uv_buf_init(&b[0], b.size());
  int r = uv_udp_try_send(&udp_client_, &buf, 1,
                          (const struct sockaddr *)&groupcastaddr_);
  if (r < 0) {
    perror("uv_udp_try_send error");
  }
  printf("try_send: %lu\n", buf.len);
  return r;
}

void ServiceDiscover::Recv(const std::string &topic, RecvCallback callback) {
  std::lock_guard<std::mutex> lg(mutex_);
  callbacks_[topic].push_back(callback);
}

inline void pinger_close_cb(uv_handle_t *handle) {}

void ServiceDiscover::Stop() {
  uv_close((uv_handle_t *)&udp_server_, pinger_close_cb);
}