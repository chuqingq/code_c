#include <iostream>
#include <memory>
 
struct Vec3
{
    int x, y, z;

    Vec3() : x(0), y(0), z(1) {
        std::cout << "Vec3()" << std::endl;
    }

    Vec3(int x, int y, int z) :x(x), y(y), z(z) { }

    ~Vec3() {
        std::cout << "~Vec3()" << std::endl;
    }
    
    friend std::ostream& operator<<(std::ostream& os, Vec3& v) {
        return os << '{' << "x:" << v.x << " y:" << v.y << " z:" << v.z  << '}';
    }
};

class MyClass {
    public:
    std::unique_ptr<Vec3> v;
    MyClass(): v(std::make_unique<Vec3>()) {

    }
};
 
int main()
{
    // 使用默认构造函数。
    std::unique_ptr<Vec3> v1 = std::make_unique<Vec3>();
    // 使用匹配这些参数的构造函数
    std::unique_ptr<Vec3> v2 = std::make_unique<Vec3>(0, 1, 2);
    // 创建指向 5 个元素数组的 unique_ptr 
    std::unique_ptr<Vec3[]> v3 = std::make_unique<Vec3[]>(5);
 
    std::cout << "make_unique<Vec3>():      " << *v1 << '\n'
              << "make_unique<Vec3>(0,1,2): " << *v2 << '\n'
              << "make_unique<Vec3[]>(5):   " << '\n';
    for (int i = 0; i < 5; i++) {
        std::cout << "     " << v3[i] << '\n';
    }

    MyClass my;
    std::cout << (my.v.get() == nullptr) << "\n";
}
/*
1、一共6个构造和析构
2、析构最后会调用
3、成员可以是unique_ptr

chuqq@cqqmbpr ~/t/c/c/20181003_make_unique> ./a.out
Vec3()
Vec3()
Vec3()
Vec3()
Vec3()
Vec3()
make_unique<Vec3>():      {x:0 y:0 z:1}
make_unique<Vec3>(0,1,2): {x:0 y:1 z:2}
make_unique<Vec3[]>(5):
     {x:0 y:0 z:1}
     {x:0 y:0 z:1}
     {x:0 y:0 z:1}
     {x:0 y:0 z:1}
     {x:0 y:0 z:1}
Vec3()
0
~Vec3()
~Vec3()
~Vec3()
~Vec3()
~Vec3()
~Vec3()
~Vec3()
~Vec3()
*/