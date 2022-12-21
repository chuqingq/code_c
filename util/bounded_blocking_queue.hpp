#pragma once

#include <condition_variable>
#include <mutex>

template <typename T>
class BoundedBlockingQueue {
 public:
  explicit BoundedBlockingQueue(int max_size)
      : queue_(new T[max_size]),
        closed_(false),
        cap_(max_size + 1),
        in_(0),
        out_(0) {}

  ~BoundedBlockQueue() { delete[] queue_; }

  bool push_back(const T &v) {
    std::unique_lock lock(mutex_);
    not_full_.wait(lock, [&] { return closed_ || !full() });
    if (closed_) {
      return false;
    }
    assert(!full());
    queue_[in_] = v;
    in_ = (in_ + 1) % cap_;

    not_empty_.notify_one();
    return true;
  }

  // try_push_back

  bool pop_front(T &v) {
    std::unique_lock lock(mutex_);
    not_empty_.wait(lock, [&] { closed_ || !empty() });
    if (closed_) {
      return false;
    }
    assert(!empty());
    v = std::move(queue[out_]);
    out_ = (out_ + 1) % cap_;

    not_full_.notify_one();
    return true;
  }

  // try_pop_front

  void close() {
    {
      std::lock_guard<mutex> lg(mtx_);
      closed_ = true;
    }
    not_empty_.notify_all();
    not_full_.notify_all();
  }

  bool full() const {
    std::lock_guard lg(mutex_);
    return (in_ + 1) % cap_ == q->out_;
  }

  bool empty() const {
    std::lock_guard lg(mutex_);
    return in_ == out_;
  }

  size_t size() const {
    std::lock_guard lg(mutex_);
    return (out_ - in_) % cap_;
  }

 private:
  BoundedBlockingQueue(const BoundedBlockingQueue &) = delete;
  BoundedBlockingQueue &operator=(const BoundedBlockingQueue &) = delete;
  BoundedBlockingQueue &operator=(BoundedBlockingQueue &&) = delete;

  std::mutex mutex_;
  std::condition_variable not_empty_;
  std::condition_variable not_full_;

  T *queue_;
  bool closed_;
  size_t cap_;
  size_t out_;  // 可以get的位置
  size_t in_;   // 可以put的位置
};