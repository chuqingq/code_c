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
  // constructor
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
  uv_udp_t udp_;
  uv_udp_t udpc_;
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
  buf->base = ((ServiceDiscover *)tcp->data)->buffer_;
  buf->len = size;
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
  /* No data here means something went wrong */
  //   assert(nread > 0);
  if (nread > 0) {
    // TODO
    printf("read_cb: %ld\n", nread);
    // printf("read_cb: %lu\n", buf->len);
    std::string topic("topic");

    printf("1111\n");
    std::lock_guard<std::mutex> lg(discover->mutex_);
    printf("2222\n");
    printf("%ld\n", discover->callbacks_[topic].size());
    for (auto cb : discover->callbacks_[topic]) {
      cb(topic, *buf);
    }
  }

  //   if (buf && !(flags & UV_UDP_MMSG_CHUNK)) buf_free(buf);  // TODO ?
}

ServiceDiscover::ServiceDiscover(uv_loop_t *loop) {
  int r = uv_udp_init(loop, &udp_);
  if (r < 0) {
    perror("uv_udp_init error");
    throw;
  }
  udp_.data = this;
  uv_udp_init(loop, &udpc_);
  udpc_.data = this;

  struct sockaddr_in addr;
  uv_ip4_addr("0.0.0.0", kGroupcastPort, &addr);

  r = uv_ip4_addr(kGroupcastIp, kGroupcastPort, &groupcastaddr_);
  if (r < 0) {
    perror("uv_ip4_addr error");
    throw;
  }

  r = uv_udp_bind(&udp_, (struct sockaddr *)&addr, UV_UDP_REUSEADDR);
  if (r < 0) {
    perror("uv_udp_bind error");
    throw;
  }

  r = uv_udp_set_membership(&udp_, kGroupcastIp, NULL, UV_JOIN_GROUP);
  if (r < 0) {
    perror("uv_udp_set_membership error");
    throw;
  }

  r = uv_udp_recv_start(&udp_, buf_alloc, pinger_read_cb);
  if (r < 0) {
    perror("uv_udp_recv_start error");
    throw;
  }
}

// int ServiceDiscover::Send(const Buffer &b) {
//   // TODO
// }

int ServiceDiscover::Send(const std::string &topic, const uv_buf_t &value) {
  auto buf = uv_buf_init(value.base, value.len);
  int r = uv_udp_try_send(&udpc_, &buf, 1,
                          (const struct sockaddr *)&groupcastaddr_);
  if (r < 0) {
    perror("uv_udp_try_send error");
  }
  printf("try_send: %lu\n", buf.len);
  return r;
}

void ServiceDiscover::Recv(const std::string &topic, RecvCallback callback) {
  std::lock_guard<std::mutex> lg(mutex_);
  // TODO
  callbacks_[topic].push_back(callback);
  printf("%ld\n", callbacks_[topic].size());
}

static void pinger_close_cb(uv_handle_t *handle) {}

void ServiceDiscover::Stop() {
  uv_close((uv_handle_t *)&udp_, pinger_close_cb);
}