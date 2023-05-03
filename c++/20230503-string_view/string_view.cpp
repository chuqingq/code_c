#include <iostream>
#include <string_view>

int main() {
    std::string str("123456123");

    std::string_view sv1(&str[0], 3);
    std::string_view sv2(&str[6], 3);
    std::string_view sv3(str.substr(0, 3));

    printf("str: %p\n", str.data());
    printf("sv1: %p\n", sv1.data());
    printf("sv2: %p\n", sv2.data());
    printf("sv3: %p\n", sv3.data());
    return 0;
}
/*
#include <iostream>
#include <string_view>

int main() {
    std::string str("123456123");

    std::string_view sv1(&str[0], 3);
    std::string_view sv2(&str[6], 3);
    std::string_view sv3(str.substr(0, 3));

    printf("str: %p\n", str.data());
    printf("sv1: %p\n", sv1.data());
    printf("sv2: %p\n", sv2.data());
    printf("sv3: %p\n", sv3.data());
    return 0;
}
*/
