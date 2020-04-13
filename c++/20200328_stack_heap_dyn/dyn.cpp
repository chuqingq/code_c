// 想做一个数据结构，类似vector，但是根据需要的长度决定在栈或堆上分配
// 例如长度小于4KB时，在栈上分配；否则在堆上分配

#include <iostream>
#include <memory>
#include <vector>
using namespace std;

// 不能用模板常量参数，因为那是编译期才确定的
// 不能是模板函数
// 只能是宏

#include <cstdlib>    // std::size_t
#include <functional> // std::function

class ScopeGuard
{
public:
    ScopeGuard(std::function<void()> &&ptr)
        : ptr_(move(ptr))
    {
    }

    void dismiss() noexcept
    {
        dismissed_ = true;
    }

    ~ScopeGuard()
    {
        if (!dismissed_)
        {
            ptr_();
        }
    }

private:
    ScopeGuard(const ScopeGuard &) = delete;
    void *operator new(std::size_t) = delete;
    void operator delete(void *) = delete;

    std::function<void()> ptr_;
    bool dismissed_;
};

#define MAKE_DYN(type, symbol, size) \
    type* symbol; \
    ScopeGuard _guard([&] { free(symbol); }); \
    if (size <= 4096) { \
        symbol = (type *)alloca(size); \
        printf("alloca %p\n", symbol); \
	_guard.dismiss(); \
    } \
    else { \
        symbol = (type *) malloc(size); \
        printf("malloc %p\n", symbol); \
    }
        

int main() {
    {
        MAKE_DYN(char, a, 100); // char a[100]
        printf("%p\n", a);
        a[0] = 'a';
        a[100-1] = 'b';
    }

    {
        MAKE_DYN(char, b, 5000);
        printf("%p\n", b);
        b[5000-1] = 'c';
    }

    return 0;
}
/*
~/temp $ g++ dyn.cpp -g -fsanitize=address
~/temp $ ./a.out 
alloca 0x7ffe4b154d20
0x7ffe4b154d20
malloc 0x622000000100
0x622000000100
~/temp $ valgrind ./a.out
==38179== Memcheck, a memory error detector
==38179== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==38179== Using Valgrind-3.15.0 and LibVEX; rerun with -h for copyright info
==38179== Command: ./a.out
==38179== 
==38179==ASan runtime does not come first in initial library list; you should either link runtime to your application or manually preload it with LD_PRELOAD.
==38179== 
==38179== HEAP SUMMARY:
==38179==     in use at exit: 0 bytes in 0 blocks
==38179==   total heap usage: 0 allocs, 0 frees, 0 bytes allocated
==38179== 
==38179== All heap blocks were freed -- no leaks are possible
==38179== 
==38179== For lists of detected and suppressed errors, rerun with: -s
==38179== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
*/
