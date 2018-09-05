#include <iostream>
#include <functional>
using namespace std;

/*
void g(function<void()> f) {
    f();
}
*/

void g(void (*f)()) {
    f();
}

int main()
{
    auto f = []() { cout << "hello, world" << endl; };
    // cout << typeid(f).name() << endl;
    // cout << f << endl;
    g(f);

    int a = 10;
    auto f2 = [&]() { cout << "hello: " << a << endl; };
    g(f2);
    return 0;
}
/*
lambda如果想作为参数传给其他函数，一般使用模板，可以参考std::for_earch()
*/
