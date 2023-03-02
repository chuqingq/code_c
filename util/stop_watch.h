#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

static inline uint64_t nstime(void) {
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  return ((uint64_t)ts.tv_sec) * 1e9 + ts.tv_nsec;
}

typedef struct {
  uint64_t start;
} stop_watch_t;

static inline void stop_watch_start(stop_watch_t *watch) {
  watch->start = nstime();
}

static inline void stop_watch_print(stop_watch_t *watch, uint64_t ops) {
  uint64_t duration = nstime() - watch->start;
  printf("StopWatch: total %lu ns; average %lu ns/op.\n", duration,
         duration / ops);
}

static inline uint64_t ustime(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (uint64_t)((uint64_t)tv.tv_sec) * 1e6 + tv.tv_usec;
}
