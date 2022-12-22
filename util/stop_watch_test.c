#include "stop_watch.h"

#include <unistd.h>

int main() {
  stop_watch_t w;
  stop_watch_start(&w);
  sleep(1);
  stop_watch_print(&w, 1);
  return 0;
}

/*
$ gcc -o stop_watch_test -Wall stop_watch_test.c
*/