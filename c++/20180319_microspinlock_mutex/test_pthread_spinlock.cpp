// g++ test_pthread_spinlock.cpp -o test_pthread_spinlock -std=c++14 -O3 -pthread

#include <iostream>
#include <thread>
#include <chrono>


int main() {
    pthread_spinlock_t lock;

    pthread_spin_lock(&lock);
    auto start = std::chrono::steady_clock::now();

    std::thread t([&] {
        pthread_spin_lock(&lock);
        auto stop = std::chrono::steady_clock::now();
        std::cout << "elapsed: " << std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count() << " ns" << std::endl;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(0));

    start = std::chrono::steady_clock::now();
    // lock.unlock();
    pthread_spin_unlock(&lock);

    t.join();
    return 0;
}

/*
chuqq@chuqq-hp:~/temp/codeeveryday/c++/20180319_microspinlock_mutex$ g++ test_pthread_spinlock.cpp -std=c++14 -pthread
chuqq@chuqq-hp:~/temp/codeeveryday/c++/20180319_microspinlock_mutex$ ./a.out 
elapsed: 713 ns
结论：经常会卡住，sleep 1秒，可能会过很久也不触发。

结论2：pthread_spinlock比pthread_mutex快一倍。   https://www.xuebuyuan.com/916222.html
*/
