#include <thread>

#include "cansocket.hpp"
#include "stop_watch.hpp"

const int LOOPS = 2000;
struct canfd_frame frame;
StopWatch watch;

void loop(const char* name) {
  CanSocket can;
  can.Open(name);

  for (auto i = 0;; i++) {
    if ((i % LOOPS) == 0) {
      watch.print(LOOPS);
      watch.start();
    }
    can.Receive(&frame);
    can.Send(&frame);
  }
}

int main() {
  frame.can_id = 291;
  memcpy(frame.data, "\x11\x22\x33\x44\x55\x66", 6);
  frame.len = 6;

  std::thread thread1([]() { loop("can0"); });
  std::thread thread2([]() { loop("can1"); });

  CanSocket can;
  can.Open("can0");

  can.Send(&frame);
  thread1.join();
  thread2.join();

  return 0;
}