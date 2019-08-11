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

int main()
{
    int r = check("234");
    printf("%d\n", r);
    return 0;
}

/*
g++ test.cpp
./a.out
3
 */
