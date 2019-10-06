#include <iostream>
#include <memory>
#include <fstream>
#include <set>

using namespace std;

template <typename T>
constexpr int check(const T *str)
{
    for (int i = 0;; i++)
    {
        if (str[i] == '\0')
        {
            return i;
        }
    }
}

// output function that requires a compile-time constant, for testing
template <int n>
struct constN
{
    constN() { std::cout << n << '\n'; }
};

void success()
{
    // int r = check("234");
    // printf("check success: %d\n", r);
    constN<check("123")> a;
}

void error()
{
    // string a = string("123") + "456";
    // const char *str = a.c_str();
    // // int r = check(str);
    // // printf("check error: %d\n", r);
    // constN<check(str)> a;

    string a = string("123");
    constN<check(a.c_str())> a;
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
