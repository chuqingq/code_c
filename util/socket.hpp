#pragma once

#include <sys/socket.h>

inline void setKeepalive(int fd, int keepalive, int keepalive_idle,
                         int keepalive_interval, int keepalive_count) {
  int ret;

  ret =
      setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepalive, sizeof(int));
  if (ret != 0) {
    LOGE("set keepalive failed.");
  }
  logd("setsockopt(SO_KEEPALIVE): {}", keepalive);

  ret = setsockopt(fd, SOL_TCP, TCP_KEEPIDLE, (void *)&keepalive_idle,
                   sizeof(int));
  if (ret != 0) {
    LOGE("set keepalive_idle failed.");
  }
  logd("setsockopt(TCP_KEEPIDLE): {}", keepalive_idle);

  ret = setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, (void *)&keepalive_interval,
                   sizeof(int));
  if (ret != 0) {
    LOGE("set keepalive_interval failed.");
  }
  logd("setsockopt(TCP_KEEPINTVL): {}", keepalive_interval);

  ret = setsockopt(fd, SOL_TCP, TCP_KEEPCNT, (void *)&keepalive_count,
                   sizeof(int));
  if (ret != 0) {
    LOGE("set keepalive_count failed.");
  }
  logd("setsockopt(TCP_KEEPCNT): {}", keepalive_count);
}