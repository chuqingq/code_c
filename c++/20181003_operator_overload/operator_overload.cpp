//  "a<b"的重载的三种方法

#include <iostream>
#include <string>
using namespace std;

// // 方法1：作为成员函数重载（常见）
// class Person{
//     string name;
//     int age;
// public:
//     Person(const char* name, int age):name(name),age(age){}
//     bool operator<(const Person& b);
 
// };
// bool Person::operator<(const Person& b)
// {
//     //作为成员函数时，*this即为左操作数a
//     return this->age < b.age;
// }
 
// // 方法2：作为友元函数重载
// class Person{
//     string name;
//     int age;
// public:
//     Person(const char* name, int age):name(name),age(age){}
//     friend bool operator<(const Person& a,const Person& b);
 
// };
// bool operator<(const Person& a,const Person& b)
// {
//     return a.age < b.age;
// }
 
// 方法3：作为普通函数重载（不推荐）
class Person{
public://注意，重载运算符为普通函数时，使用到的类成员必须为public
    string name;
    int age;
public:
    Person(const char* name, int age):name(name),age(age){}
 
};
bool operator<(const Person& a,const Person& b)
{
    return a.age < b.age;
}

////

int main() {
    Person p1("1", 1);
    Person p2("2", 2);
    std::cout << (p1 < p2) << std::endl;

    return 0;
}