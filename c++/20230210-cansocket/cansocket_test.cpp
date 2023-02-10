#include "cansocket.hpp"

int main() {
  CanSocket can0;
  can0.Open("can0");

  CanSocket can1;
  can1.Open("can1");

  struct canfd_frame frame0;
  frame0.can_id = 291;
  memcpy(frame0.data, "\x11\x22\x33\x44\x55\x66", 6);
  frame0.len = 6;

  can0.Send(&frame0);

  struct canfd_frame frame1;
  can1.Receive(&frame1);

  return 0;
}