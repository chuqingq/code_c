#include <iostream>
#include <string_view>

int main() {
    std::string str("123456123");

    std::string_view sv1(&str[0], 3);
    std::string_view sv2(&str[6], 3);
    std::string_view sv3(str.substr(0, 3));

    printf("str: %p\n", str.data());
    printf("end: %p\n", str.data()+str.size());
    printf("sv1: %p\n", sv1.data());
    printf("sv2: %p\n", sv2.data());
    printf("sv3: %p\n", sv3.data());
    return 0;
}
/*
$ g++ string_view.cpp -Wall -std=c++17
不告警。

$ clang++ string_view.cpp -Wall -std=c++17
string_view.cpp:9:26: warning: object backing the pointer will be destroyed at the end of the full-expression [-Wdangling-gsl]
    std::string_view sv3(str.substr(0, 3));
                         ^~~~~~~~~~~~~~~~
1 warning generated.

$ ./a.out
str: 0x7ffe3abdfda0
end: 0x7ffe3abdfda9
sv1: 0x7ffe3abdfda0
sv2: 0x7ffe3abdfda6
sv3: 0x7ffe3abdfdc0

sv3依赖的string，应该已经失效了。
*/
