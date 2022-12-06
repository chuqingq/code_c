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
    // bool need_release_;
    // ~Buffer()
    // {
    //     if (need_release_)
    //     {
    //         delete data_;
    //         data_ = NULL;
    //         size_ = 0;
    //     }
    // }
};

class Message;
class MessagePublisher;
class MessageSubscriber;

class Message
{
public:
    static inline MessagePublisher *NewMessagePublisher(const std::string &topic);
    static inline MessageSubscriber *NewMessageSubscriber(const std::string &topic, const std::function<void(Buffer &buffer)> &callback);

private:
    static std::map<std::string, MessagePublisher *> &GetTopics()
    {
        static std::map<std::string, MessagePublisher *> topics;
        return topics;
    }
    static std::mutex &GetTopicsMutex()
    {
        static std::mutex topics_mutex_;
        return topics_mutex_;
    }

    friend class MessagePublisher;
};

class MessagePublisher
{
public:
    inline void Stop();

    ~MessagePublisher()
    {
        Stop();
    }
    // int Acquire(Buffer &buf){
    //     // TODO
    // };
    inline int Publish(Buffer &b);

private:
    std::string topic_;
    // Buffer buffer_;
    std::mutex mutex_;
    std::condition_variable cond_;

    std::set<MessageSubscriber *> subscribers_;
    std::mutex subscribers_mutex_;

    friend class Message;
    friend class MessageSubscriber;
};

class MessageSubscriber
{
public:
    // MessageSubscriber() : stop_(false) {}

    void Stop()
    {
        {
            std::lock_guard guard(publisher_->mutex_);
            stop_ = true;
            publisher_->subscribers_.erase(this); // TODO 放到上面
        }
        publisher_->cond_.notify_all(); // TODO 想避免唤醒所有sub

        thread_.join();

        {
            std::lock_guard guard(publisher_->mutex_);
        }
    }
    ~MessageSubscriber()
    {
        Stop();
    }

    // typedef const std::function<void(int)> &SubCallback;
private:
    MessageSubscriber(MessagePublisher *publisher, const std::function<void(Buffer &)> &callback)
    {
        stop_ = false;
        publisher_ = publisher;
        callback_ = callback;
        auto loop = [&]
        {
            while (true)
            {
                std::unique_lock lock(publisher_->mutex_);
                if (buffer_ == nullptr)
                {
                    publisher_->cond_.wait(lock);
                    std::cout << (void *)this << " wait()\n";
                    if (stop_)
                    {
                        std::cout << (void *)this << " sub stopped\n";
                        return;
                    }
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

    MessagePublisher *publisher_;
    std::shared_ptr<Buffer> buffer_;
    bool stop_;
    // std::mutex stop_mutex_;
    // std::condition_variable stop_cond_;

    std::thread thread_;
    std::function<void(Buffer &)> callback_;

    friend class Message;
    friend class MessagePublisher;
};

// Message

MessagePublisher *Message::NewMessagePublisher(const std::string &topic)
{
    std::lock_guard guard(Message::GetTopicsMutex());

    auto it = Message::GetTopics().find(topic);
    if (it != Message::GetTopics().end())
    {
        if (!it->second->topic_.empty())
        {
            throw;
        }
        it->second->topic_ = topic;
        return it->second;
    }
    else
    {
        auto pub = new MessagePublisher();
        pub->topic_ = topic;
        Message::GetTopics()[topic] = pub;
        return pub;
    }
}

int MessagePublisher::Publish(Buffer &b)
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

MessageSubscriber *Message::NewMessageSubscriber(const std::string &topic, const std::function<void(Buffer &buffer)> &callback)
{
    std::map<std::string, MessagePublisher *>::iterator it;
    {
        std::lock_guard guard(Message::GetTopicsMutex());
        it = Message::GetTopics().find(topic);
        if (it == Message::GetTopics().end())
        {
            auto pub = new MessagePublisher();
            // pub.topic_ = topic;
            Message::GetTopics()[topic] = pub;
            it = Message::GetTopics().find(topic);
        }
    }

    {
        std::lock_guard guard(it->second->subscribers_mutex_);
        auto sub = new MessageSubscriber(it->second, callback);
        it->second->subscribers_.insert(sub);
        return sub;
    }
}

// MessagePublisher

void MessagePublisher::Stop()
{
    {
        std::lock_guard guard(Message::GetTopicsMutex());
        Message::GetTopics().erase(topic_);
    }

    {
        std::lock_guard guard(mutex_);
        for (auto sub : subscribers_)
        {
            sub->stop_ = true;
        }
    }

    cond_.notify_all();

    // delete this;
}
