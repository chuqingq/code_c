#include <assert.h>
#include <unistd.h>

#include <thread>

#include "message.hpp"
#include "stop_watch.hpp"

const uint64_t COUNT = 40000;
// const uint64_t COUNT = 2;

static StopWatch stopwatch;

char value[8] = "\0";

// static Buffer buffer;
static MessagePublisher *pub1, *pub2;
static MessageSubscriber *sub1, *sub2;

static inline void Send(MessagePublisher *pub, uint64_t i) {
  Buffer b;
  pub->Acquire(&b);
  *(uint64_t *)b.data_ = i;
  pub->Publish(b);
}

void RecvProc(MessageSubscriber *sub, const Buffer &buf) {
  uint64_t i = *(uint64_t *)buf.data_;  // TODO 转换
  //   std::cout << "RecvProc: " << i << std::endl;
  if (i > COUNT) {
    std::cout << (void *)sub << " sub stopping\n";
    stopwatch.print(COUNT);
    sub->Stop();
    // 本sub停掉后，继续发送，确保另一个线程的sub和pub可以停掉。
  }
  MessagePublisher *pub = pub2;
  auto next = i;
  if (sub == sub2) {
    pub = pub1;
    next += 1;
  }

  Send(pub, next);
  // 本pub发送完成后，如果i超过COUNT则停掉。
  if (i > COUNT) pub->Stop();
}

int multi_thread_main() {
  // init
  const std::string ip("127.0.0.1");
  const std::string topic1("mytopic1");
  const std::string topic2("mytopic2");
  const int port1 = 10001;
  const int port2 = 10002;

  std::jthread thread2([&] {
    uv_loop_t loop;
    uv_loop_init(&loop);

    MessageSubscriber sub11(topic1, ip, port1, RecvProc, &loop);
    sub1 = &sub11;

    MessagePublisher pub22(topic2, &loop);
    pub22.AddSub(ip, port2);
    pub2 = &pub22;

    uv_run(&loop, UV_RUN_DEFAULT);
    int r = uv_loop_close(&loop);
    assert(r == 0);
  });

  MessagePublisher pub11(topic1);
  pub11.AddSub(ip, port1);
  pub1 = &pub11;
  MessageSubscriber sub22(topic2, ip, port2, RecvProc, uv_default_loop());
  sub2 = &sub22;

  usleep(200);  // TODO

  // 开始测试
  stopwatch.start();

  Send(pub1, 1);
  std::cout << "## main thread after Send(1)\n";

  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  return 0;
}

int main() {
  multi_thread_main();
  return 0;
}
// multi_thread_main: TODO
// StopWatch: total 1908617835 ns; average 38172 ns/loop.
// 家中matebook wsl：
// StopWatch: total 3263650097 ns; average 65273 ns/loop.
