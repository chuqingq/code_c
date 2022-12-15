#pragma once

#include <uv.h>

#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <shared_mutex>
#include <string>
#include <vector>

#include "shm.hpp"

struct Buffer {
  char *data_;
  size_t size_;

  Buffer() : data_(nullptr), size_(0) {}
  Buffer(char *data, size_t size) : data_(data), size_(size) {}
};

static void pinger_close_cb(uv_handle_t *handle) {}

const static char kNotifyRead = 'r';
const static char kNotifyOpen = 'o';

class MessagePublisher {
 public:
  inline MessagePublisher(const std::string &topic,
                          uv_loop_t *loop = uv_default_loop());
  inline int AddSub(const std::string &ip, int port);
  inline int Acquire(Buffer *buf);
  inline void Publish(const Buffer &buf);
  inline void Stop();
  inline ~MessagePublisher();

 private:
  uv_loop_t *loop_;
  uv_udp_t udp_;
  std::vector<struct sockaddr_in> subs_;
  ShmBlock shm_;
};

const int kBlockNum = 8;
const int kBlockSize = 4 * 1024 * 1024;

MessagePublisher::MessagePublisher(const std::string &topic, uv_loop_t *loop)
    : loop_(loop), shm_(topic, kBlockNum, kBlockSize) {
  int r = uv_udp_init(loop_, &udp_);
  if (r) {
    std::cout << "uv_udp_init error\n";
    throw "uv_udp_init error";
  }
  if (!shm_.Create()) {
    throw;
  }
}

int MessagePublisher::AddSub(const std::string &ip, int port) {
  struct sockaddr_in addr;
  int r = uv_ip4_addr(ip.c_str(), port, &addr);
  if (r) {
    std::cout << "uv_ip4_addr error\n";
    return -1;
  }

  // 通知sub已经创建。
  // 如果sub先创建而pub未创建，需要这里通知sub重新打开共享内存
  uv_buf_t b = uv_buf_init((char *)&kNotifyOpen, sizeof(kNotifyOpen));
  r = uv_udp_try_send(&udp_, &b, 1, (const struct sockaddr *)&addr);
  if (r < 0) {
    perror("uv_udp_try_send error");
  }

  subs_.push_back(addr);
  return 0;
}

int MessagePublisher::Acquire(Buffer *buf) {
  if (buf->size_ > kBlockSize) throw "buf size exceeds kBlockSize";
  buf->data_ = (char *)shm_.AcquireBlockToWrite();
  buf->size_ = kBlockSize;
  return 0;
}

void MessagePublisher::Publish(const Buffer &buf) {
  shm_.ReleaseWrittenBlock(buf.data_);
  // 通知subs TODO 后面要改成把addr放在State中？
  for (const auto sub : subs_) {
    uv_buf_t b = uv_buf_init((char *)&kNotifyRead, sizeof(kNotifyRead));
    int r = uv_udp_try_send(&udp_, &b, 1, (const struct sockaddr *)&sub);
    if (r < 0) {
      perror("uv_udp_try_send error");
    }
  }
}

void MessagePublisher::Stop() {
  uv_close((uv_handle_t *)&udp_, pinger_close_cb);
  loop_ = nullptr;
  // shm_会自动析构
  std::cout << (void *)this << " pub stop\n";
}

MessagePublisher::~MessagePublisher() {
  if (loop_) Stop();
}

class MessageSubscriber {
 public:
  typedef std::function<void(MessageSubscriber *sub, const Buffer &buffer)>
      Callback;

  inline MessageSubscriber(const std::string &topic, const std::string &ip,
                           int port, const Callback &callback,
                           uv_loop_t *loop = uv_default_loop());
  ~MessageSubscriber() {
    if (loop_) Stop();
  }
  inline void Stop();

 private:
  uv_udp_t udp_;
  struct sockaddr_in addr_;
  uv_loop_t *loop_;
  char buffer_[64 * 1024];
  Callback callback_;
  ShmBlock shm_;

  friend inline void buf_alloc(uv_handle_t *tcp, size_t size, uv_buf_t *buf);
  friend inline void pinger_read_cb(uv_udp_t *udp, ssize_t nread,
                                    const uv_buf_t *buf,
                                    const struct sockaddr *addr,
                                    unsigned flags);
};

inline void buf_alloc(uv_handle_t *tcp, size_t size, uv_buf_t *buf) {
  buf->base = ((MessageSubscriber *)tcp->data)->buffer_;
  buf->len = size;
}

static void buf_free(const uv_buf_t *buf) {}

inline void pinger_read_cb(uv_udp_t *udp, ssize_t nread, const uv_buf_t *buf,
                           const struct sockaddr *addr, unsigned flags) {
  if (nread == 0) {
    return;
  }
  MessageSubscriber *sub = (MessageSubscriber *)udp->data;
  /* No data here means something went wrong */
  assert(nread > 0);
  if (nread > 0) {
    switch (*buf->base) {
      case kNotifyRead: {
        auto b = sub->shm_.AcquireBlockToRead();
        if (!b) return;
        Buffer buf((char *)b, kBlockSize);  // TODO 大小未标识，暂时用kBlockSize
        // std::cout << "read_cb: " << buf->len << "\n";
        sub->callback_(sub, buf);
        sub->shm_.ReleaseReadBlock(b);
      } break;
      case kNotifyOpen: {
        sub->shm_.Stop();
        bool success = sub->shm_.Open();
        std::cout << "reopen: " << success << std::endl;
      } break;
    }
  }

  if (buf && !(flags & UV_UDP_MMSG_CHUNK)) buf_free(buf);  // TODO ?
}

MessageSubscriber::MessageSubscriber(const std::string &topic,
                                     const std::string &ip, int port,
                                     const Callback &callback, uv_loop_t *loop)
    : loop_(loop), callback_(callback), shm_(topic, kBlockNum, kBlockSize) {
  // udp
  int r = uv_udp_init(loop_, &udp_);
  if (r) {
    std::cout << "uv_udp_init error\n";
    throw;
  }
  udp_.data = this;

  r = uv_ip4_addr(ip.c_str(), port, &addr_);
  if (r) {
    std::cout << "uv_ip4_addr error\n";
  }
  r = uv_udp_bind(&udp_, (const struct sockaddr *)&addr_, 0);
  if (r) {
    perror("bind error");
    std::cout << "uv_udp_bind error: " << r << std::endl;
  }
  r = uv_udp_recv_start(&udp_, buf_alloc, pinger_read_cb);
  if (r) {
    std::cout << "uv_udp_recv_start error\n";
  }

  //   std::cout << (void *)&async_subscriber_ << " subscriber async: " <<
  //   std::endl;
  // 尝试打开共享内存
  shm_.Open();
}

void MessageSubscriber::Stop() {
  uv_close((uv_handle_t *)&udp_, pinger_close_cb);
  loop_ = nullptr;
  // shm_会自动析构
  std::cout << "sub stop\n";
}
