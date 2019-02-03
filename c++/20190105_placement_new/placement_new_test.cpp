// #include <iostream> // 这行用valgrind检查有内存泄漏，应该是valgrind误报
#include <memory>

class Book {
    int a;
};


int main() {
    // char array[sizeof(Book)]; // 不要14行的释放，不会有内存泄漏
    char *array = new char[sizeof(Book)];
    Book *b = new(array) Book(); // 使用array这块内存来创建Book对象
    b->~Book(); // 析构，但是不会释放array的空间
    delete[] array; // 加上这行，valgrind仍然有内存泄漏，应该是valgrind误报
    return 0;
}
