#include <future>
#include <iostream>
#include <chrono>
#include <thread>

using namespace std::literals;

int main() {
    std::packaged_task<int()> pt([]{
        std::cout << "child start" << std::endl;
        std::this_thread::sleep_for(5s);
        std::cout << "child stop" << std::endl;
        return 0;
    });

    auto fut = pt.get_future();
    std::cout << "main: before pt()" << std::endl;
    std::thread t(std::move(pt));
    // t.join();
    std::cout << "main: after pt()" << std::endl;
    std::cout << fut.get() << std::endl;
    t.join();
    return 0;
}

/*
chuqq@cqqmbpr ~/t/c/c/20181002_thread> ./a.out
main: before pt()
main: after pt()
child start
child stop
0
*/