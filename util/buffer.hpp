#pragma once

#include <vector>

inline void concatBuffer(std::vector<char> const &src, std::vector<char> *dst) {
  size_t dstOldSize = dst->size();
  dst->resize(dstOldSize + src.size());
  std::copy(src.begin(), src.end(), dst->begin() + dstOldSize);
}