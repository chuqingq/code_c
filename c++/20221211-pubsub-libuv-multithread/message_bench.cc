#include <unistd.h>

#include <thread>

#include "message.hpp"
#include "stop_watch.hpp"

const uint64_t COUNT = 50000;
// const uint64_t COUNT = 2;

static StopWatch stopwatch;

static std::shared_ptr<Buffer> buffer(new Buffer((char *)1, 0));
static MessagePublisher *pub1, *pub2;
static MessageSubscriber *sub1, *sub2;

static inline void Send(MessagePublisher *pub, uint64_t i) {
  buffer->data_ = (char *)i;
  // uint64_t ii = (uint64_t)buffer.data_;
  // std::cout << (void *)pub << " pub send: " << ii << std::endl;
  pub->Publish(buffer);
}

void RecvProc(MessageSubscriber *sub, std::shared_ptr<Buffer> buffer) {
  uint64_t i = (uint64_t)buffer->data_;
  // std::cout << i << std::endl;
  if (i > COUNT) {
    std::cout << (void *)sub << " sub stopping\n";
    stopwatch.print(COUNT);
    pub1->Stop();
    pub2->Stop();
    sub1->Stop();
    sub2->Stop();
    return;
    // exit(0);
  }
  MessagePublisher *pub = pub2;
  auto next = i;
  if (sub == sub2) {
    pub = pub1;
    next += 1;
  }

  buffer->data_ = (char *)next;
  pub->Publish(buffer);
}

int single_thread_main() {
  // init
  const std::string topic1("mytopic1");
  MessagePublisher pub11(topic1);
  pub1 = &pub11;
  MessageSubscriber sub11(topic1, RecvProc);
  sub1 = &sub11;

  const std::string topic2("mytopic2");
  MessagePublisher pub22(topic2);
  pub2 = &pub22;
  MessageSubscriber sub22(topic2, RecvProc);
  sub2 = &sub22;

  // 开始测试
  stopwatch.start();

  Send(pub1, 1);
  std::cout << "## main thread after Send(1)\n";

  uv_run(uv_default_loop(), UV_RUN_DEFAULT);

  return 0;
}

int multi_thread_main() {
  // init
  const std::string topic1("mytopic1");
  const std::string topic2("mytopic2");

  std::jthread thread2([&] {
    uv_loop_t loop;
    uv_loop_init(&loop);

    MessageSubscriber sub11(topic1, RecvProc, &loop);
    sub1 = &sub11;

    MessagePublisher pub22(topic2);
    pub2 = &pub22;

    uv_run(&loop, UV_RUN_DEFAULT);
    uv_loop_close(&loop);
    std::cout << "thread2 exit\n";
  });

  MessagePublisher pub11(topic1);
  pub1 = &pub11;
  MessageSubscriber sub22(topic2, RecvProc, uv_default_loop());
  sub2 = &sub22;

  sleep(1);  // TODO

  // 开始测试
  stopwatch.start();

  Send(pub1, 1);
  std::cout << "## main thread after Send(1)\n";

  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  return 0;
}

int main() {
  multi_thread_main();
  return single_thread_main();
}

// multi_thread_main:
// StopWatch: total 1509671305 ns; average 30193 ns/loop.
// single_thread_main:
// StopWatch: total 50184822 ns; average 1003 ns/loop.
