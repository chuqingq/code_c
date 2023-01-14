#include <assert.h>
#include <unistd.h>

#include <thread>

#include "message.hpp"
#include "stop_watch.hpp"

const uint64_t COUNT = 100000;
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
    // std::cout << (void *)sub << " sub stopping\n";
    if (sub == sub2) {
      stopwatch.print(COUNT);
    }
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
  const std::string topic1("mytopic1");
  const std::string topic2("mytopic2");

  // thread1通过pub1通知thread2，thread2通过sub1接收；
  // thread2再通过pub2通知thread1，thread1再通过sub2接收；
  // 所以thread1持有pub1和sub2；thread2持有sub1和pub2
  MessagePublisher pub11(topic1);
  pub1 = &pub11;
  MessageSubscriber sub22(topic2, RecvProc);
  sub2 = &sub22;

  uv_loop_t loop;
  uv_loop_init(&loop);

  MessagePublisher pub22(topic2, &loop);
  pub2 = &pub22;

  MessageSubscriber sub11(topic1, RecvProc, &loop);
  sub1 = &sub11;

  std::thread thread2([&] {
    uv_run(&loop, UV_RUN_DEFAULT);
    int r = uv_loop_close(&loop);
    assert(r == 0);
  });

  uv_run(uv_default_loop(), UV_RUN_NOWAIT);

  // 开始测试
  stopwatch.start();

  Send(pub1, 1);
  std::cout << "## main thread after Send(1)\n";

  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  thread2.join();
  return 0;
}

int main() {
  while (1) {
    multi_thread_main();
    std::cout << "====\n";
  }
  return 0;
}
// StopWatch: total 3672980834 ns; average 36729 ns/loop.