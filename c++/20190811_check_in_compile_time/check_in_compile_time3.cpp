#include <iostream>
#include <memory>
#include <fstream>
#include <set>

using namespace std;

constexpr int check(const char *str)
{
    for (int i = 0;; i++)
    {
        if (str[i] == '\0')
        {
            return i > 10;
        }
    }
}

/*
// output function that requires a compile-time constant, for testing
template <const char *p>
struct constN
{
    constN() { std::cout << p << '\n'; }
};
*/
void success()
{
    // int r = check("234");
    // printf("check success: %d\n", r);
    static_assert(check("213"), "unexpected error");
}

void error()
{
    // string a = string("123") + "456";
    // const char *str = a.c_str();
    // // int r = check(str);
    // // printf("check error: %d\n", r);
    // constN<check(str)> a;

    string a = string("123");
    // static_assert(check(a.c_str()), "expected error");
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
