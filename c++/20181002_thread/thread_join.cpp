#include <thread>
#include <iostream>
#include <chrono>

using namespace std::literals;

int main() {
    std::thread t([] {
        std::cout << "child start" << std::endl;
        std::this_thread::sleep_for(5s);
        std::cout << "child stop" << std::endl;
    });

    // t.detach();
    t.join();
    std::cout << "main stop" << std::endl;
    return 0;
}

/*
编译： g++ thread_joni.cpp -pthread
场景0：创建thread对象后，会自动运行；pthread_create也是一样；

场景1：如果不调用t.detach()和t.join()，则会抛异常
linux：
$ ./a.out
main stopchild start

terminate called without an active exception
Aborted (core dumped)

macos：
chuqq@cqqmbpr ~/t/c/c/20181002_thread> ./a.out
main stop
libc++abi.dylib: terminatingchild start

fish: './a.out' terminated by signal SIGABRT (Abort)

场景2：如果调用detach，不会抛异常，也不会等5秒，直接结束:
$ date;./a.out;date
Wed Mar 13 22:50:18 DST 2019
main stop
child start
child start
Wed Mar 13 22:50:18 DST 2019

场景3：如果调用join，不会抛异常，会等待5秒后结束：
$ date;./a.out ;date
Wed Mar 13 22:51:10 DST 2019
child start
child stop
main stop
Wed Mar 13 22:51:15 DST 2019

场景4：如果编译时静态链接 g++ thread_join.cpp -static -pthread ，且调用detach，会在detach时抛异常sigsegv：
$ g++ thread_join.cpp -static -pthread
$ gdb ./a.out
(gdb) r
Starting program: /mnt/d/work/temp/code_c/c++/20181002_thread/a.out
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".
[New Thread 0x7fffff7e0700 (LWP 2110)]
child start

Thread 1 "a.out" received signal SIGSEGV, Segmentation fault.
0x0000000000000000 in ?? ()
(gdb) bt
#0  0x0000000000000000 in ?? ()
#1  0x000000000042ad63 in std::thread::join() ()
#2  0x000000000040112c in main ()
(gdb)

参考： https://stackoverflow.com/questions/35116327/when-g-static-link-pthread-cause-segmentation-fault-why

*/
