#pragma once

#include <stdio.h>

#include <string>

// 执行命令
inline int executeCmd(const std::string &cmd, std::string *res) {
  FILE *ptr = popen(cmd.c_str(), "r");
  if (ptr == NULL) {
    return -1;
  }

  char buf[128];
  while (fgets(buf, sizeof(buf), ptr) != NULL) {
    *res += std::string(buf);
  }

  pclose(ptr);
  ptr = NULL;
  return 0;
}