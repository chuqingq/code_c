#include "log.hpp"

int main() {
  logd("debug log");
  logi("info log");
  loge("error log");

  SetLogLevel(kLogLevelError);
  logd("debug log");
  logi("info log");
  loge("error log");
  return 0;
}