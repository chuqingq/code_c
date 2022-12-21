#ifndef STOP_WATCH_H
#define STOP_WATCH_H

#include <sys/time.h>

static long long ustime(void) {
  struct timeval tv;
  long long ust;
  gettimeofday(&tv, NULL);
  return (long long)((long long)tv.tv_sec) * 1e6 + tv.tv_usec;
  return ust;
}

#include <time.h>

static long long nstime(void) {
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  return ((long long)ts.tv_sec) * 1e9 + ts.tv_nsec;
}

#endif