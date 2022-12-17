#include "file.hpp"

#include "catch.hpp"

TEST_CASE("readFile + writeFile", "util_test") {
  const std::string file("util.test");
  const std::vector<char> content{'1', '2', '3', '4'};

  REQUIRE(writeFile(file, content) == 0);

  std::vector<char> res;
  REQUIRE(readFile(file, &res) == 0);

  REQUIRE(content == res);

  // 删除文件
  REQUIRE(deleteFile(file) == 0);
  REQUIRE(!fileExists(file));
}