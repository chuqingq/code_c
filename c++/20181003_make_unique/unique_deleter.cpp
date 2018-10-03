#include <iostream>

class MyClass {
    public:
    MyClass() {
        std::cout << "MyClass()" << std::endl;
    }

    ~MyClass() {
        std::cout << "~MyClass()" << std::endl;
    }
};

int main() {
    std::unique_ptr<MyClass, std::function<void(MyClass*)>> a(new MyClass(), [](MyClass * my) {
        std::cout << "deleter" << std::endl;
        delete my;
    });
}