#pragma once

#include <string>
#include <map>
#include <set>
#include <shared_mutex>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>
#include <iostream>

struct Buffer
{
    char *data_;
    size_t size_;

    Buffer() {}
    Buffer(char *data, size_t size) : data_(data), size_(size) {}
};

class MessagePublisher;
class MessageSubscriber;

class MessagePublisherDelegate
{
public:
    MessagePublisherDelegate() {}
    ~MessagePublisherDelegate() { Stop(); }

private:
    static inline std::shared_ptr<MessagePublisherDelegate> GetOrNew(const std::string &topic);

    inline void Stop();

    // int Acquire(Buffer &buf){
    //     // TODO
    // };
    inline int Publish(Buffer &b);

private:
    typedef std::map<std::string, std::shared_ptr<MessagePublisherDelegate>> TopicMap;
    static inline TopicMap &GetTopics();
    static inline std::mutex &GetTopicsMutex();

    std::string topic_;
    // Buffer buffer_;
    std::mutex mutex_;
    std::condition_variable cond_;

    std::set<MessageSubscriber *> subscribers_;
    std::mutex subscribers_mutex_;

    friend class Message;
    friend class MessagePublisher;
    friend class MessageSubscriber;
};

class MessagePublisher
{
public:
    MessagePublisher(const std::string &topic)
    {
        delegate_ = MessagePublisherDelegate::GetOrNew(topic);
    }

    void Publish(Buffer &buffer)
    {
        delegate_->Publish(buffer);
    }

    void Stop()
    {
        delegate_->Stop();
    }

    ~MessagePublisher()
    {
        delegate_->Stop();
    }

private:
    std::shared_ptr<MessagePublisherDelegate> delegate_;
};

class MessageSubscriber
{
public:
    typedef std::function<void(Buffer &buffer)> Callback;

    MessageSubscriber(const std::string &topic, const Callback &callback);

    void Stop()
    {
        {
            std::lock_guard guard(publisher_->mutex_);
            stop_ = true;
            publisher_->subscribers_.erase(this);
        }
        publisher_->cond_.notify_all();
    }

    void WaitStop()
    {
        if (thread_.joinable())
            thread_.join();
    }

    ~MessageSubscriber()
    {
        if (!stop_)
        {
            Stop();
        }
        WaitStop();
    }

private:
    std::shared_ptr<MessagePublisherDelegate> publisher_;
    std::shared_ptr<Buffer> buffer_;
    bool stop_;

    std::thread thread_;
    Callback callback_;

    friend class Message;
    friend class MessagePublisherDelegate;
};

// MessagePublisherDelegate

MessagePublisherDelegate::TopicMap &MessagePublisherDelegate::GetTopics()
{
    static TopicMap topics;
    return topics;
}

std::mutex &MessagePublisherDelegate::GetTopicsMutex()
{
    static std::mutex topics_mutex_;
    return topics_mutex_;
}

// MessagePublisherDelegate::GetOrNew 根据topic获取Delegate，如果没有则新建。被Publisher和Subscriber调用
std::shared_ptr<MessagePublisherDelegate> MessagePublisherDelegate::GetOrNew(const std::string &topic)
{
    std::lock_guard guard(GetTopicsMutex());

    auto it = GetTopics().find(topic);
    if (it != GetTopics().end())
    {
        return it->second;
    }
    else
    {
        std::shared_ptr<MessagePublisherDelegate> delegate(new MessagePublisherDelegate());
        delegate->topic_ = topic;
        GetTopics()[topic] = delegate;
        return delegate;
    }
}

int MessagePublisherDelegate::Publish(Buffer &b)
{
    std::lock_guard guard(mutex_);
    std::shared_ptr<Buffer> buf(new Buffer(b.data_, b.size_));
    for (auto sub : subscribers_)
    {
        sub->buffer_ = buf;
    }
    cond_.notify_all();
    return 0;
};

void MessagePublisherDelegate::Stop()
{
    {
        std::lock_guard guard(GetTopicsMutex());
        GetTopics().erase(topic_);
    }

    {
        std::lock_guard guard(mutex_);
        for (auto sub : subscribers_)
        {
            sub->stop_ = true;
        }
    }
    cond_.notify_all();

    // delete this; // TODO 改成不一定在堆上分配
}

// MessageSubscriber

MessageSubscriber::MessageSubscriber(const std::string &topic, const Callback &callback)
{
    auto delegate = MessagePublisherDelegate::GetOrNew(topic);
    {
        std::lock_guard guard(delegate->subscribers_mutex_);
        delegate->subscribers_.insert(this);
    }

    stop_ = false;
    publisher_ = delegate;
    callback_ = callback;

    auto loop = [&]
    {
        while (true)
        {
            std::unique_lock lock(publisher_->mutex_);
            if (stop_)
            {
                std::cout << (void *)this << " sub stopped\n";
                return;
            }
            if (buffer_ == nullptr)
            {
                publisher_->cond_.wait(lock);
                // std::cout << (void *)this << " wait()\n";
            }
            std::shared_ptr<Buffer> buf;
            buf.swap(buffer_);
            lock.unlock();

            if (buf != nullptr && buf->data_ != nullptr)
            {
                callback_(*buf.get());
            }
        }
    };

    std::thread th(loop);
    thread_ = std::move(th);
}

// DONE 确保Publisher和Subscriber可以使用堆或栈分配
// DONE MessagePublisher使用MessagePublisherDelegate
// TODO 使用libuv，不为subscriber创建独立的线程
