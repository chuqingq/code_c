#include "ScopeGuard.hpp"

#include <iostream>
using namespace std;

#undef NDEBUG
#include <cassert>

int main()
{
    int i = 0;

    // 普通用法
    {
        ScopeGuard a([&] {
            i++;
        });
    }
    assert(i == 1);

    // 匿名
    {
        ScopeGuard([&] {
            i++;
        });
    }
    assert(i == 2);

    // dismiss
    {
        ScopeGuard c([&] {
            i = 3;
        });
        c.dismiss();
    }
    assert(i == 2);

    return 0;
}
