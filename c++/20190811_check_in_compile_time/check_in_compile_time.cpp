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

void success()
{
    int r = check("234");
    printf("check success: %d\n", r);
}

void error()
{
    string a = string("123") + "456";
    const char *str = a.c_str();
    int r = check(str);
    printf("check error: %d\n", r);
}

int main()
{
    success();
    error();
    return 0;
}

/*
$ g++ check_in_compile_time.cpp
check success: 3
check error: 6
 */
