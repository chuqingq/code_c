#include <iostream>
#include <memory>

struct Base {
  Base() { std::cout << "  Base::Base()\n"; }
  // Note: non-virtual destructor is OK here
  ~Base() { std::cout << "  Base::~Base()\n"; }
};

int main() {
  Base *b = NULL;
  {
    std::shared_ptr<Base> p = std::make_shared<Base>();
    {
      std::shared_ptr<Base> p2 = p;
      b = p2.get();
      printf("b: %p\n", b);
      std::cout << "leave inner" << std::endl;
      std::cout << "use_count: " << p.use_count() << std::endl;
    }
    std::cout << "leave outer" << std::endl;
    std::cout << "use_count: " << p.use_count() << std::endl;
  } // p在这里析构
  std::cout << "out of outer" << std::endl;
  // std::cout << "use_count: " << p.use_count() << std::endl;

  (void)b; // 这里应该已经是野指针了
  printf("b: %p\n", b);
  std::cout << "leave main" << std::endl;
  return 0;
}

/*
输出：
  Base::Base()
b: 0x7feffd400658
leave inner
use_count: 2
leave outer
use_count: 1
  Base::~Base()
out of outer
b: 0x7feffd400658
leave main
*/
