#include <iostream>
#include <memory>
#include <fstream>
#include <set>

using namespace std;

constexpr bool check(const char *str)
{
    for (int i = 0;; i++)
    {
        if (str[i] == '\0')
        {
            return i > 3;
        }
    }
}

// output function that requires a compile-time constant, for testing
template <bool b>
struct myassert
{
};

template <>
struct myassert<true>
{
};

void success()
{
	myassert<check("123")> m;
}

void error()
{
    string a = string("123");
    myassert<check(a.c_str())> m;
}

int main()
{
    success();
    error();
    return 0;
}

/*
43行编译错误，说明确实尝试编译器计算结果。
 */
