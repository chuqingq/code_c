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

struct Buffer {
  char *data_;
  size_t size_;

  Buffer() : data_(nullptr), size_(0) {}
  Buffer(char *data, size_t size) : data_(data), size_(size) {}
};

// inline uv_loop_t *GetThreadLoop() {
// }

// inline void SetThreadLoop(uv_loop_t *loop) {
// }

class MessagePublisher;
class MessageSubscriber;

class MessagePublisherDelegate {
 public:
  MessagePublisherDelegate() {}
  ~MessagePublisherDelegate() {}

 private:
  static inline std::shared_ptr<MessagePublisherDelegate> GetOrNew(
      const std::string &topic);
  inline void Stop();
  inline void Publish(uv_udp_t *udp, std::shared_ptr<Buffer> &buf);

 private:
  typedef std::map<std::string, std::shared_ptr<MessagePublisherDelegate>>
      TopicMap;
  static inline TopicMap &GetTopics();
  static inline std::mutex &GetTopicsMutex();

  std::string topic_;
  std::set<MessageSubscriber *> subscribers_;
  std::mutex subscribers_mutex_;

  friend class Message;
  friend class MessagePublisher;
  friend class MessageSubscriber;
  friend inline void async_subscriber_cb(uv_async_t *);
};

static void pinger_close_cb(uv_handle_t *handle) {}

class MessagePublisher {
 public:
  MessagePublisher(const std::string &topic,
                   uv_loop_t *loop = uv_default_loop())
      : loop_(loop) {
    delegate_ = MessagePublisherDelegate::GetOrNew(topic);
    int r = uv_udp_init(loop_, &udp_);
    if (r) {
      std::cout << "uv_udp_init error\n";
      throw;
    }
  }
  int AddSub(const std::string &ip, int port) {
    struct sockaddr_in addr;
    int r = uv_ip4_addr(ip.c_str(), port, &addr);
    if (r) {
      std::cout << "uv_ip4_addr error\n";
      return -1;
    }
    subs_.push_back(addr);
    return 0;
  }
  void Publish(std::shared_ptr<Buffer> &buf) {
    for (const auto sub : subs_) {
      uv_buf_t b = uv_buf_init(buf->data_, buf->size_);
      //   std::cout << b.len << std::endl;
      int r = uv_udp_try_send(&udp_, &b, 1, (const struct sockaddr *)&sub);
      if (r < 0) {
        perror("uv_udp_try_send error");
      }
      //   std::cout << (void *)this << " pub try_send: " << buf->size_ <<
      //   std::endl;
    }
  }
  void Stop() { delegate_->Stop(); }
  ~MessagePublisher() { uv_close((uv_handle_t *)&udp_, pinger_close_cb); }

 private:
  std::shared_ptr<MessagePublisherDelegate> delegate_;
  uv_loop_t *loop_;
  uv_udp_t udp_;
  std::vector<struct sockaddr_in> subs_;
};

class MessageSubscriber {
 public:
  typedef std::function<void(MessageSubscriber *sub,
                             std::shared_ptr<Buffer> buffer)>
      Callback;

  inline MessageSubscriber(const std::string &topic, const std::string &ip,
                           int port, const Callback &callback,
                           uv_loop_t *loop = uv_default_loop());
  ~MessageSubscriber() {
    if (!stop_.load()) Stop();
  }
  inline void Stop();

 private:
  inline void Post(uv_udp_t *udp, std::shared_ptr<Buffer> &buf);
  // 确保释放最后一个sub时可以释放delegate
  std::shared_ptr<MessagePublisherDelegate> publisher_;  // TODO

  uv_udp_t udp_;
  struct sockaddr_in addr_;
  uv_loop_t *loop_;
  uv_async_t async_subscriber_;
  std::atomic<bool> stop_;
  std::atomic<std::shared_ptr<Buffer>> buffer_;
  char slab_[8];  // TODO
  Callback callback_;

  friend class Message;
  friend class MessagePublisherDelegate;
  friend inline void async_subscriber_cb(uv_async_t *);
  friend inline void buf_alloc(uv_handle_t *tcp, size_t size, uv_buf_t *buf);
  friend inline void pinger_read_cb(uv_udp_t *udp, ssize_t nread,
                                    const uv_buf_t *buf,
                                    const struct sockaddr *addr,
                                    unsigned flags);
};

// MessagePublisherDelegate

MessagePublisherDelegate::TopicMap &MessagePublisherDelegate::GetTopics() {
  static TopicMap topics;
  return topics;
}

std::mutex &MessagePublisherDelegate::GetTopicsMutex() {
  static std::mutex topics_mutex_;
  return topics_mutex_;
}

// MessagePublisherDelegate::GetOrNew
// 根据topic获取Delegate，如果没有则新建。被Publisher和Subscriber调用
std::shared_ptr<MessagePublisherDelegate> MessagePublisherDelegate::GetOrNew(
    const std::string &topic) {
  std::lock_guard<std::mutex> guard(GetTopicsMutex());

  auto it = GetTopics().find(topic);
  if (it != GetTopics().end()) {
    return it->second;
  } else {
    std::shared_ptr<MessagePublisherDelegate> delegate(
        new MessagePublisherDelegate());
    delegate->topic_ = topic;
    GetTopics()[topic] = delegate;
    return delegate;
  }
}

void MessagePublisherDelegate::Publish(uv_udp_t *udp,
                                       std::shared_ptr<Buffer> &buf) {
  std::lock_guard<std::mutex> guard(subscribers_mutex_);
  for (auto sub : subscribers_) {
    sub->Post(udp, buf);
  }
}

void MessagePublisherDelegate::Stop() {
  std::cout << (void *)this << " pub stopped\n";
  {
    std::lock_guard<std::mutex> guard(GetTopicsMutex());
    GetTopics().erase(topic_);
  }
}

// MessageSubscriber

void async_subscriber_cb(uv_async_t *handle) {
  //   std::cout << (void *)handle << " async_subscriber_cb\n";
  auto sub = (MessageSubscriber *)handle->data;

  if (sub->stop_.load()) {
    std::cout << (void *)handle->data << " sub stopping1\n";
    uv_close((uv_handle_t *)&sub->async_subscriber_, NULL);
    std::lock_guard<std::mutex> guard(sub->publisher_->subscribers_mutex_);
    sub->publisher_->subscribers_.erase(sub);
    return;
  }

  auto buf = sub->buffer_.load();
  if (buf != nullptr) {
    sub->callback_(sub, buf);
  }
}

inline void buf_alloc(uv_handle_t *tcp, size_t size, uv_buf_t *buf) {
  buf->base = (char *)&((MessageSubscriber *)tcp->data)->slab_;
  buf->len = sizeof(((MessageSubscriber *)tcp->data)->slab_);
}

static void buf_free(const uv_buf_t *buf) {}

inline void pinger_read_cb(uv_udp_t *udp, ssize_t nread, const uv_buf_t *buf,
                           const struct sockaddr *addr, unsigned flags) {
  if (nread == 0) {
    return;
  }
  MessageSubscriber *sub = (MessageSubscriber *)udp->data;
  //   std::cout << (void *)sub << " pinger_read_cb: nread = " << nread <<
  //   std::endl;

  /* No data here means something went wrong */
  //   ASSERT(nread > 0);
  if (nread > 0) {
    std::shared_ptr<Buffer> b(new Buffer(buf->base, buf->len));
    sub->callback_(sub, b);
  }

  if (buf && !(flags & UV_UDP_MMSG_CHUNK)) buf_free(buf);  // TODO ?
}

MessageSubscriber::MessageSubscriber(const std::string &topic,
                                     const std::string &ip, int port,
                                     const Callback &callback, uv_loop_t *loop)
    : loop_(loop), stop_(false), callback_(callback) {
  // async
  uv_async_init(loop_, &async_subscriber_, async_subscriber_cb);
  async_subscriber_.data = (void *)this;

  auto delegate = MessagePublisherDelegate::GetOrNew(topic);
  publisher_ = delegate;
  {
    std::lock_guard<std::mutex> guard(delegate->subscribers_mutex_);
    delegate->subscribers_.insert(this);
  }

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
}

void MessageSubscriber::Post(uv_udp_t *udp, std::shared_ptr<Buffer> &buf) {
  //   buffer_.store(buf);
  //   uv_async_send(&async_subscriber_);
  uv_buf_t b = uv_buf_init(buf->data_, buf->size_);
  //   std::cout << b.len << std::endl;

  int r = uv_udp_try_send(udp, &b, 1, (const struct sockaddr *)&addr_);
  if (r < 0) {
    perror("uv_udp_try_send error");
  }
}

void MessageSubscriber::Stop() {
  stop_.store(true);
  uv_async_send(&async_subscriber_);
  uv_close((uv_handle_t *)&udp_, pinger_close_cb);
  std::cout << "sub stop\n";
}

// DONE 确保Publisher和Subscriber可以使用堆或栈分配
// DONE MessagePublisher使用MessagePublisherDelegate
// DONE 使用libuv，不为subscriber创建独立的线程
