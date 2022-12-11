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
  inline void Publish(std::shared_ptr<Buffer> &buf);

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

class MessagePublisher {
 public:
  MessagePublisher(const std::string &topic) {
    delegate_ = MessagePublisherDelegate::GetOrNew(topic);
  }
  void Publish(std::shared_ptr<Buffer> &buf) { delegate_->Publish(buf); }
  void Stop() { delegate_->Stop(); }
  ~MessagePublisher() {}

 private:
  std::shared_ptr<MessagePublisherDelegate> delegate_;
};

class MessageSubscriber {
 public:
  typedef std::function<void(MessageSubscriber *sub,
                             std::shared_ptr<Buffer> buffer)>
      Callback;

  inline MessageSubscriber(const std::string &topic, const Callback &callback,
                           uv_loop_t *loop = uv_default_loop());
  ~MessageSubscriber() {
    if (!stop_.load()) Stop();
  }
  inline void Stop();

 private:
  inline void Post(std::shared_ptr<Buffer> &buf);
  // 确保释放最后一个sub时可以释放delegate
  std::shared_ptr<MessagePublisherDelegate> publisher_;  // TODO

  uv_loop_t *loop_;
  uv_async_t async_subscriber_;
  std::atomic<bool> stop_;
  std::atomic<std::shared_ptr<Buffer>> buffer_;
  Callback callback_;

  friend class Message;
  friend class MessagePublisherDelegate;
  friend inline void async_subscriber_cb(uv_async_t *);
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

void MessagePublisherDelegate::Publish(std::shared_ptr<Buffer> &buf) {
  std::lock_guard<std::mutex> guard(subscribers_mutex_);
  for (auto sub : subscribers_) {
    sub->Post(buf);
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

MessageSubscriber::MessageSubscriber(const std::string &topic,
                                     const Callback &callback,
                                     uv_loop_t *loop) {
  loop_ = loop;
  stop_.store(false);
  callback_ = callback;
  uv_async_init(loop_, &async_subscriber_, async_subscriber_cb);
  async_subscriber_.data = (void *)this;

  auto delegate = MessagePublisherDelegate::GetOrNew(topic);
  publisher_ = delegate;
  {
    std::lock_guard<std::mutex> guard(delegate->subscribers_mutex_);
    delegate->subscribers_.insert(this);
  }
  //   std::cout << (void *)&async_subscriber_ << " subscriber async: " <<
  //   std::endl;
}

void MessageSubscriber::Post(std::shared_ptr<Buffer> &buf) {
  buffer_.store(buf);
  uv_async_send(&async_subscriber_);
}

void MessageSubscriber::Stop() {
  stop_.store(true);
  uv_async_send(&async_subscriber_);
}

// DONE 确保Publisher和Subscriber可以使用堆或栈分配
// DONE MessagePublisher使用MessagePublisherDelegate
// DONE 使用libuv，不为subscriber创建独立的线程
