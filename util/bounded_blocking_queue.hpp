#pragma once

#include <cassert>
#include <condition_variable>
#include <mutex>

template <typename T>
class BoundedBlockingQueue {
 public:
  explicit BoundedBlockingQueue(int max_elems)
      : data_(new T[max_elems]),
        capacity_(max_elems + 1),
        in_(0),
        out_(0),
        closed_(false) {}

  ~BoundedBlockingQueue() { delete[] data_; }

  bool push_back(const T &v) {
    std::unique_lock<std::mutex> lock(mutex_);
    not_full_.wait(lock, [&] { return closed_ || !full(); });
    if (closed_) {
      return false;
    }
    assert(!full());
    data_[in_] = v;
    in_ = (in_ + 1) % capacity_;

    not_empty_.notify_one();
    return true;
  }

  // TODO try_push_back

  bool pop_front(T &v) {
    std::unique_lock<std::mutex> lock(mutex_);
    not_empty_.wait(lock, [&] { return closed_ || !empty(); });
    if (closed_) {
      return false;
    }
    assert(!empty());
    v = std::move(data_[out_]);
    out_ = (out_ + 1) % capacity_;

    not_full_.notify_one();
    return true;
  }

  // TODO try_pop_front

  void close() {
    {
      std::lock_guard<std::mutex> lg(mutex_);
      closed_ = true;
    }
    not_empty_.notify_all();
    not_full_.notify_all();
  }

  // TODO size_t size() const

 private:
  BoundedBlockingQueue(const BoundedBlockingQueue &) = delete;
  BoundedBlockingQueue &operator=(const BoundedBlockingQueue &) = delete;
  BoundedBlockingQueue &operator=(BoundedBlockingQueue &&) = delete;

  bool full() const { return (in_ + 1) % capacity_ == out_; }

  bool empty() const { return in_ == out_; }

  std::mutex mutex_;
  std::condition_variable not_empty_;
  std::condition_variable not_full_;

  T *data_;
  size_t capacity_;
  size_t in_;   // 可以put的位置
  size_t out_;  // 可以get的位置
  bool closed_;
};
