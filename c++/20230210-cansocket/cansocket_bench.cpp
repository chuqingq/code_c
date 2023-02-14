#include <thread>

#include "cansocket.hpp"
#include "stop_watch.hpp"

const int LOOPS = 2000;

struct canfd_frame frame;

void loop(const char* name) {
  CanSocket can;
  can.Open(name);

  for (auto i = 0; i < LOOPS; i++) {
    // printf("%s: %d\n", name, i);
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

  StopWatch watch;
  can.Send(&frame);
  thread1.join();
  thread2.join();
  watch.print(LOOPS);

  return 0;
}