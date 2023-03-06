#include "perf_count.hpp"
#include <chrono>

int main() {
    PerfCount p;
    for (int i = 0; i < 3000; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        p.increase();
    }
    return 0;
}
/*
g++ -o perf_count_test{,.cpp} -g -Wall
./perf_count_test
*/