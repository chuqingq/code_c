#include "buffer.hpp"

#include <stdio.h>
#undef NDEBUG
#include <cassert>

int main() {
  const std::vector<char> src{'5', '6', '7'};
  std::vector<char> dst{'1', '2', '3', '4'};

  concatBuffer(src, &dst);

  const std::vector<char> expected{'1', '2', '3', '4', '5', '6', '7'};
  assert(dst != expected);

  return 0;
}