#include "file.hpp"

#include <stdio.h>
#undef NDEBUG
#include <cassert>

// "readFile + writeFile", "util_test"
int main() {
  const std::string file("util.test");
  const std::vector<char> content{'1', '2', '3', '4'};

  // write file
  assert(writeFile(file, content) == 0);

  // read file
  std::vector<char> res;
  assert(readFile(file, &res) == 0);
  assert(content == res);

  // delete file
  assert(deleteFile(file) == 0);
  assert(!fileExists(file));

  return 0;
}