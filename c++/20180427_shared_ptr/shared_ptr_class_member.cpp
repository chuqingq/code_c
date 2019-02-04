// 验证智能指针在类中作为成员变量，也可以随着类的析构自动释放

#include <memory>
#include <iostream>

struct Base
{
    Base() { std::cout << "  Base::Base()\n"; }
    // Note: non-virtual destructor is OK here
    ~Base() { std::cout << "  Base::~Base()\n"; }
};

class My
{
public:
    std::shared_ptr<Base> b;
};

int main() {
    {
        My m;
        printf("base: %p\n", m.b.get()); // 此时未初始化，是空指针
        m.b = std::make_shared<Base>();
        std::cout << "before leave inner" << std::endl;
    } // 这里析构m时会调用b.~Base()
    std::cout << "after leave inner" << std::endl;

    return 0;
}

/*
输出：
base: 0x0
  Base::Base()
before leave inner
  Base::~Base()
after leave inner
*/
