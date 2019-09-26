#include <iomanip>
#include <iostream>
#include <sstream>

int main()
{
  // // Sep 16 2019 01:47:30 // "2011-Februar-18 23:12:34");
  const char str[] = __DATE__ " " __TIME__;
  std::cout << str << std::endl;

  std::istringstream ss(str);
  std::tm t = {};
  ss >> std::get_time(&t, "%b %d %Y %H:%M:%S");

  std::cout << std::put_time(&t, "%Y-%Om-%Od %H:%M:%S") << std::endl;

  // std::cout << std::localtime(&t) << std::endl;
  return 0;
}

/*
Sep 17 2019 06:25:13
2019-09-17_06-25-13
TODO 应该是14点
*/
