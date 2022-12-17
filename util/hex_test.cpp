#include "hex.hpp"

#include "catch.hpp"

static void hexCodecTest(std::vector<char> const &d,
                         std::vector<char> const &h) {
  std::vector<char> t;

  hexEncode(d, &t);
  REQUIRE(t == h);

  hexDecode(h, &t);
  REQUIRE(t == d);
}

TEST_CASE("hexEncode + hexDecode", "util_test") {
  std::vector<char> d{1, 18, 10, 27, -1};
  std::vector<char> h{'0', '1', '1', '2', '0', 'A', '1', 'B', 'F', 'F', '\0'};
  hexCodecTest(d, h);

  std::vector<char> d1{1, 2, 3, 127, -100};
  std::vector<char> h1{'0', '1', '0', '2', '0', '3', '7', 'F', '9', 'C', '\0'};
  hexCodecTest(d1, h1);
}

TEST_CASE("hex_encode + hex_decode", "util_test") {
  uint8_t src[]{1, 18, 10, 27, 255};
  char dst[]{'0', '1', '1', '2', '0', 'A', '1', 'B', 'F', 'F', '\0'};

  uint8_t src2[sizeof(src)];
  char dst2[sizeof(dst)];

  hex_encode(src, sizeof(src), dst2, sizeof(dst2));
  REQUIRE(memcmp(dst, dst2, sizeof(dst)) == 0);

  hex_decode(dst, src2, sizeof(src2));
  REQUIRE(memcmp(src, src2, sizeof(src)) == 0);
}