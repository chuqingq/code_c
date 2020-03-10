#include "fmt/format.h"
#include <iostream>
#include <string>
using namespace std;

struct my {
	int a;
	int b;
	string s;
};

template <>
struct fmt::formatter<my> {
  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const my& d, FormatContext& ctx) {
    return format_to(ctx.out(), "{}-{}-{}", d.a, d.b, d.s);
  }
};

int main()
{
	std::string s = fmt::format("{0}is{1}", "abra",12);
	std::cout << s <<std::endl;

	my m;
	m.a = 10;
	m.b = 999;
	m.s = "hello world";
	cout << fmt::format("m: {}", m) << endl;

	return 0;
}
/*
~/temp $ g++ main_fmt.cpp -lfmt -std=c++14
~/temp $ ./a.out
abrais12
m: 10-999-hello world
*/

