#pragma once

#include <uv.h>

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

    Buffer() : data_(nullptr), size_(0) {}
    Buffer(char *data, size_t size) : data_(data), size_(size) {}
};

class MessagePublisher;
class MessageSubscriber;

class MessagePublisherDelegate
{
public:
    MessagePublisherDelegate();
    ~MessagePublisherDelegate() { Stop(); }

private:
    static inline std::shared_ptr<MessagePublisherDelegate> GetOrNew(const std::string &topic);
    inline void Stop();
    inline int Publish(Buffer &b);

private:
    typedef std::map<std::string, std::shared_ptr<MessagePublisherDelegate>> TopicMap;
    static inline TopicMap &GetTopics();
    static inline std::mutex &GetTopicsMutex();

    std::string topic_;
    std::shared_ptr<Buffer> buffer_;

    std::mutex mutex_; // TODO 是否必要
    std::condition_variable cond_;

    bool stop_;
    uv_async_t async_publisher_; // 外部线程publish，通过此字段通知

    std::set<MessageSubscriber *> subscribers_;
    std::mutex subscribers_mutex_;

    friend class Message;
    friend class MessagePublisher;
    friend class MessageSubscriber;
    friend inline void async_publisher_cb(uv_async_t *handle);
    friend inline void async_subscriber_cb(uv_async_t *);
};

class MessagePublisher
{
public:
    MessagePublisher(const std::string &topic) { delegate_ = MessagePublisherDelegate::GetOrNew(topic); }
    void Publish(Buffer &buffer) { delegate_->Publish(buffer); }
    void Stop() { delegate_->Stop(); }
    ~MessagePublisher() { delegate_->Stop(); }

private:
    std::shared_ptr<MessagePublisherDelegate> delegate_;
};

class MessageSubscriber
{
public:
    typedef std::function<void(Buffer &buffer)> Callback;

    MessageSubscriber(const std::string &topic, const Callback &callback);

    void Stop();
    ~MessageSubscriber()
    {
        if (!stop_)
            Stop();
    }

private:
    std::shared_ptr<MessagePublisherDelegate> publisher_;
    bool stop_;

    uv_async_t async_subscriber_;

    std::thread thread_;
    Callback callback_;

    friend class Message;
    friend class MessagePublisherDelegate;
    friend inline void async_publisher_cb(uv_async_t *);
    friend inline void async_subscriber_cb(uv_async_t *);
};

// MessagePublisherDelegate

void async_publisher_cb(uv_async_t *handle)
{
    auto delegate = (MessagePublisherDelegate *)handle->data;

    // std::unique_lock lock(delegate->mutex_);
    if (delegate->stop_)
    {
        std::cout << (void *)delegate << " sub stopped\n";
        {
            std::lock_guard guard(delegate->GetTopicsMutex());
            delegate->GetTopics().erase(delegate->topic_);
        }

        {
            // std::lock_guard guard(delegate->mutex_);
            uv_close((uv_handle_t *)&delegate->async_publisher_, NULL);
            for (auto sub : delegate->subscribers_)
            {
                sub->stop_ = true;
                uv_close((uv_handle_t *)&sub->async_subscriber_, NULL);
            }
        }
        return;
    }
    if (delegate->buffer_ == nullptr)
    {
        return;
    }
    std::shared_ptr<Buffer> buf;
    buf.swap(delegate->buffer_);
    // lock.unlock();

    if (buf != nullptr && buf->data_ != nullptr)
    {
        for (auto sub : delegate->subscribers_)
            sub->callback_(*buf.get());
    }
}

MessagePublisherDelegate::MessagePublisherDelegate() : stop_(false)
{
    uv_async_init(uv_default_loop(), &async_publisher_, async_publisher_cb);
    async_publisher_.data = (void *)this;
    std::cout << "pub async init\n";
}

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
    {
        // std::lock_guard guard(mutex_); // TODO
        std::shared_ptr<Buffer> buf(new Buffer(b.data_, b.size_));
        buffer_ = buf;
    }
    // TODO timer或者其他API优先级更高？
    uv_async_send(&async_publisher_);
    return 0;
};

void MessagePublisherDelegate::Stop()
{
    {
        // std::lock_guard guard(mutex_);
        stop_ = true;
    }
    uv_async_send(&async_publisher_);
}

// MessageSubscriber

void async_subscriber_cb(uv_async_t *handle)
{
    auto sub = (MessageSubscriber *)handle->data;

    if (sub->stop_)
        uv_close((uv_handle_t *)&sub->async_subscriber_, NULL);

    // std::lock_guard guard(sub->publisher_->subscribers_mutex_);
    sub->publisher_->subscribers_.erase(sub);
}

MessageSubscriber::MessageSubscriber(const std::string &topic, const Callback &callback)
{
    auto delegate = MessagePublisherDelegate::GetOrNew(topic);
    {
        std::lock_guard guard(delegate->subscribers_mutex_); // TODO 应该要发送async，然后由eventloop来加入到set
        delegate->subscribers_.insert(this);
    }

    stop_ = false;
    publisher_ = delegate;
    callback_ = callback;
    uv_async_init(uv_default_loop(), &async_subscriber_, NULL);
    async_subscriber_.data = (void *)this;
}

void MessageSubscriber::Stop()
{
    {
        // std::lock_guard guard(publisher_->mutex_); // TODO
        stop_ = true;
    }
    uv_async_send(&async_subscriber_);
}

// DONE 确保Publisher和Subscriber可以使用堆或栈分配
// DONE MessagePublisher使用MessagePublisherDelegate
// TODO 使用libuv，不为subscriber创建独立的线程
