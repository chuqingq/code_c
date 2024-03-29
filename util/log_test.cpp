#include "log.hpp"

int main() {
  logd("debug log %d", 123);
  logi("info log");
  logw("warn log");
  loge("error log");

  SetLogLevel(kLogLevelError);
  logd("debug log");
  logi("info log");
  logw("warn log");
  loge("error log %f", 1.2);

  return 0;
}
/*
$ g++ -o log_test{,.cpp} -Wall
*/