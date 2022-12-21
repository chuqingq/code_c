#pragma once

#include <condition_variable>
#include <mutex>

template <typename T>
class BoundedBlockingQueue {
 public:
  explicit BoundedBlockingQueue(int maxSize)
      : closed_(false), queue_(new T[maxSize]) {}

  ~BoundedBlockQueue() { delete[] queue_; }

  int put(const T &x)  // 返回成功失败
  {
    std::unique_lock lock(mutex_);
    notFull_.wait(lock, [&] { return !queue_.full() });
    if (closed) {
      return -1;
    }
    assert(!queue_.full());
    queue_.push_back(x);  // TODO
    notEmpty_.notify_one();
  }

  int put(T &&x)  // TODO ?? std::forward() // 返回成功失败
  {
    std::unique_lock lock(mutex_);
    notFull_.wait(lock, [&] { return !queue_.full() });
    if (closed) {
      return -1;
    }
    assert(!queue_.full());
    queue_.push_back(std::move(x));
    notEmpty_.notify_one();
    return 0;
  }

  int take(T &v) {
    std::unique_lock lock(mutex_);
    notEmpty_.wait(lock, [&] { !queue_.empty() });
    if (closed) {
      return -1;
    }
    assert(!queue_.empty());
        v = std::move(queue_.front()));
        queue_.pop_front();
        notFull_.notify_one();
        return 0;
  }

  void close() {
        {
          lock_guard<mutex> lk(mtx_);
          closed_ = true;
        }
        not_empty_.notify_all();
        not_full_.notify_all();
  }

  // bool empty() const
  // {
  //     std::unique_lock lock(mutex_);
  //     return queue_.empty();
  // }

  // bool full() const
  // {
  //     std::unique_lock lock(mutex_);
  //     return queue_.full();
  // }

  // size_t size() const
  // {
  //     std::unique_lock lock(mutex_);
  //     return queue_.size();
  // }

  size_t capacity() const {
        std::unique_lock lock(mutex_);
        return queue_.capacity();
  }

 private:
  BoundedBlockingQueue(const BoundedBlockingQueue &) = delete;
  BoundedBlockingQueue &operator=(const BoundedBlockingQueue &) = delete;
  BoundedBlockingQueue &operator=(BoundedBlockingQueue &&) = delete;

  std::mutex mutex_;
  std::condition_variable notEmpty_;
  std::condition_variable notFull_;
  // boost::circular_buffer<T> queue_ GUARDED_BY(mutex_); // TODO
  T *queue_;
  bool closed_;
};