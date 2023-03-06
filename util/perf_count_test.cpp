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
$ g++ -o perf_count_test{,.cpp} -g -Wall
$ ./perf_count_test
PerfCount: total 1000175597 ns; average 1093088 ns/loop.
PerfCount: total 1000342916 ns; average 1093270 ns/loop.
PerfCount: total 1000148143 ns; average 1093058 ns/loop.
PerfCount: total 1000165898 ns; average 3922219 ns/loop.
*/