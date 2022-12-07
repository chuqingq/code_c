#include "message.hpp"
#include "stop_watch.hpp"

#include <thread>
#include <unistd.h>

const uint64_t COUNT = 100000;

static StopWatch stopwatch;

static Buffer buffer;
static MessagePublisher *pub1, *pub2;
static MessageSubscriber *sub1, *sub2;

static void Send(MessagePublisher *pub, uint64_t i)
{
    buffer.data_ = (char *)i;
    // uint64_t ii = (uint64_t)buffer.data_;
    // std::cout << (void *)pub << " pub send: " << ii << std::endl;
    pub->Publish(buffer);
}

void RecvProc(MessageSubscriber *sub, Buffer &buffer)
{
    uint64_t i = (uint64_t)buffer.data_;
    if (i > COUNT)
    {
        stopwatch.print(COUNT);
        pub1->Stop();
        pub2->Stop();
        sub1->Stop();
        sub2->Stop();
        // exit(0);
    }
    if (sub == sub1)
    {
        // std::cout << (void *)sub << " sub1 recv: " << i << std::endl;
        Send(pub2, i);
    }
    else
    {
        // std::cout << (void *)sub << " sub2 recv: " << i << std::endl;
        Send(pub1, i + 1);
    }
}

int main()
{
    // init
    buffer.data_ = (char *)1;
    buffer.size_ = sizeof(buffer.data_);
    // buffer.need_release_ = false;

    const std::string topic1("mytopic1");
    MessagePublisher pub11(topic1);
    pub1 = &pub11;
    auto sub1_cb = [&](Buffer &buffer)
    {
        RecvProc(sub1, buffer);
    };
    MessageSubscriber sub11(topic1, sub1_cb);
    sub1 = &sub11;

    const std::string topic2("mytopic2");
    MessagePublisher pub22(topic2);
    pub2 = &pub22;
    auto sub2_cb = [&](Buffer &buffer)
    {
        RecvProc(sub2, buffer);
    };
    MessageSubscriber sub22(topic2, sub2_cb);
    sub2 = &sub22;

    // 开始测试
    stopwatch.start();

    Send(pub1, 1);
    std::cout << "## main thread after Send(1)\n";

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    return 0;
}

// StopWatch: total 61498892 ns; average 614 ns/loop.