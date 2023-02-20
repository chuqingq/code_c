#pragma once

#include <sstream>
#include <string>
#include <vector>

// 字符串操作
inline std::string &replace_all(std::string &str, const std::string &old_value,
                                const std::string &new_value) {
  if (old_value == new_value) {
    return str;
  }
  while (true) {
    std::string::size_type pos(0);
    if ((pos = str.find(old_value)) != std::string::npos)
      str.replace(pos, old_value.length(), new_value);
    else
      break;
  }
  return str;
}

inline std::string capture(const std::string &str, const std::string &left,
                           const std::string &right) {
  auto lpos = str.find(left);
  if (lpos == std::string::npos) {
    return "";
  }
  lpos += left.length();

  auto rpos = str.find(right, lpos);
  if (rpos == std::string::npos) {
    return "";
  }

  return str.substr(lpos, rpos - lpos);
}

inline std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  std::stringstream ss(s);
  std::string item;

  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}
