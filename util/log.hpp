#pragma

#include <stdio.h>
#include <sys/time.h>
#include <time.h>

const int kLogLevelStart = 0;
const int kLogLevelDebug = 2;
const int kLogLevelInfom = 4;
const int kLogLevelError = 6;
const int kLogLevelDisab = 9;

inline int &LogLevel() {
  static int gLogLevel = kLogLevelDebug;
  return gLogLevel;
}

inline void SetLogLevel(int level) { LogLevel() = level; }

#define log(level, fmt, args...)                                               \
  do {                                                                         \
    if (LogLevel() <= level) {                                                 \
      struct timeval current_time;                                             \
      struct tm local_tm;                                                      \
      gettimeofday(&current_time, 0);                                          \
      localtime_r(&current_time.tv_sec, &local_tm);                            \
      printf("%04d-%02d-%02d %02d:%02d:%02d.%06ld [%d %s:%d %s] " fmt "\n",    \
             local_tm.tm_year + 1900, local_tm.tm_mon + 1, local_tm.tm_mday,   \
             local_tm.tm_hour, local_tm.tm_min, local_tm.tm_sec,               \
             (long)current_time.tv_usec, LogLevel(), __FILE__, __LINE__,       \
             __func__, ##args);                                                \
    }                                                                          \
  } while (0)

#define logd(fmt, args...) log(kLogLevelDebug, fmt, ##args)
#define logi(fmt, args...) log(kLogLevelInfom, fmt, ##args)
#define loge(fmt, args...) log(kLogLevelError, fmt, ##args)
