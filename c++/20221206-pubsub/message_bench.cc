#include "message.hpp"
#include "stop_watch.hpp"

#include <thread>
#include <unistd.h>

const uint64_t COUNT = 1000;

static StopWatch stopwatch;

static Buffer buffer;
static MessagePublisher *pub1, *pub2;
static MessageSubscriber *sub1, *sub2;

static void Send(MessagePublisher *pub, uint64_t i)
{
    buffer.data_ = (char *)i;
    uint64_t ii = (uint64_t)buffer.data_;
    std::cout << (void *)pub << " pub send: " << ii << std::endl;
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
        std::cout << (void *)sub << " sub1 recv: " << i << std::endl;
        Send(pub2, i);
    }
    else
    {
        std::cout << (void *)sub << " sub2 recv: " << i << std::endl;
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
    pub1 = Message::NewMessagePublisher(topic1);
    auto sub1_cb = [&](Buffer &buffer)
    {
        RecvProc(sub1, buffer);
    };
    sub1 = Message::NewMessageSubscriber(topic1, sub1_cb);

    const std::string topic2("mytopic2");
    pub2 = Message::NewMessagePublisher(topic2);
    auto sub2_cb = [&](Buffer &buffer)
    {
        RecvProc(sub2, buffer);
    };
    sub2 = Message::NewMessageSubscriber(topic2, sub2_cb);

    // 开始测试
    stopwatch.start();

    Send(pub1, 1);

    // wait
    sub1->WaitStop();
    sub2->WaitStop();
    // while (1)
    // {
    //     // std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    //     sleep(2);
    // }

    return 0;
}

/*
编译：
make

性能测试条件：
Ubuntu 20.04 LTS in VirtualBox

性能测试结果：
终端1执行： ./bench_receiver
终端2执行： ./bench_sender
相同节点、不同进程： StopWatch: total 5111822684 ns; average 511182 ns/loop.
*/
