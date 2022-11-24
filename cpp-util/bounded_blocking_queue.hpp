#pragma once

#include <mutex>
#include <condition_variable>

template <typename T>
class BoundedBlockingQueue
{
public:
    explicit BoundedBlockingQueue(int maxSize)
        : mutex_(),
          notEmpty_(),
          notFull_(),
          queue_(maxSize) // TODO
    {
    }

    void put(const T &x)
    {
        std::unique_lock lock(mutex_);
        notFull_.wait(lock, [&]
                      { return !queue_.full() });
        assert(!queue_.full());
        queue_.push_back(x); // TODO
        notEmpty_.notify_one();
    }

    void put(T &&x) // TODO ?? std::forward()
    {
        std::unique_lock lock(mutex_);
        notFull_.wait(lock, [&]
                      { return !queue_.full() });
        assert(!queue_.full());
        queue_.push_back(std::move(x));
        notEmpty_.notify_one();
    }

    T take()
    {
        std::unique_lock lock(mutex_);
        notEmpty_.wait(lock, [&]
                       { !queue_.empty() });
        assert(!queue_.empty());
        T front(std::move(queue_.front()));
        queue_.pop_front();
        notFull_.notify_one();
        return front;
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

    size_t capacity() const
    {
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
};