#include "log.hpp"

extern void print_log();

int main() {
  logd("debug log %d", 123);
  logi("info log");
  loge("error log");
  print_log();

  SetLogLevel(kLogLevelError);
  logd("debug log");
  logi("info log");
  loge("error log %f", 1.2);

  print_log();
  return 0;
}
