#include <thread>
#include <iostream>
#include <chrono>

#include <pthread.h>

using namespace std::literals;

static void* func(void*) {
        std::cout << "child start" << std::endl;
        std::this_thread::sleep_for(5s);
        std::cout << "child stop" << std::endl;
}

int main() {
    pthread_t tid;
    pthread_create(&tid, NULL, func, NULL);
    pthread_detach(tid);
    // t.join();
    std::cout << "main stop" << std::endl;
    return 0;
}

/*
chuqq@cqqmbpr ~/t/c/c/20181002_thread> ./a.out
main stop
libc++abi.dylib: terminatingchild start

fish: './a.out' terminated by signal SIGABRT (Abort)
*/
