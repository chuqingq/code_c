#include <memory>
#include <iostream>

struct Base
{
    Base() { std::cout << "  Base::Base()\n"; }
    // Note: non-virtual destructor is OK here
    ~Base() { std::cout << "  Base::~Base()\n"; }
};

int main() {
    Base *b;
    {
        std::shared_ptr<Base> p = std::make_shared<Base>();
        {
            std::shared_ptr<Base> p2 = p;
            b = p2.get();
            std::cout << "leave inner" << std::endl;
        }
        std::cout << "leave outer" << std::endl;
    }

    (void)b;
    std::cout << "leave main" << std::endl;
    return 0;
}
