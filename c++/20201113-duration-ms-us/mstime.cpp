#include <iostream>
#include <chrono>
#include <thread>
 
void f()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

long mstime() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

int main()
{
    auto start = mstime();
    f();
    auto end = mstime();

    std::cout << "f() took " << end-start << " ms, " << std::endl;
}