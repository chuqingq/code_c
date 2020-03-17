#include "ScopeGuard.hpp"

#include <iostream>
using namespace std;

int main()
{
    cout << "0000" << endl;
    {
        ScopeGuard a([] { cout << "1111" << endl; });
    }
    cout << "2222" << endl;

    int i = 1;
    {
        ScopeGuard b([&] {
            i = 2;
            cout << "3333" << endl;
        });
    }
    cout << "4444: " << i << endl;

    return 0;
}