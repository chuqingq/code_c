#include "buffer.hpp"

#include "catch.hpp"

TEST_CASE("concatBuffer", "util_test") {
  const std::vector<char> src{'5', '6', '7'};
  std::vector<char> dst{'1', '2', '3', '4'};

  concatBuffer(src, &dst);

  const std::vector<char> expected{'1', '2', '3', '4', '5', '6', '7'};
  REQUIRE(dst == expected);
}