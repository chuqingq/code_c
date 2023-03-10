#include "perf_count.hpp"
#include <chrono>

int main() {
    PerfCount p;
    for (int i = 0; i < 3000; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        p.Incr();
    }
    std::cout << "stop\n";
    p.Stop();
    return 0;
}
/*
$ g++ -o perf_count_test{,.cpp} -g -Wall
$ ./perf_count_test
PerfCount: total 1000226685 ns; loops: 913; average 1095538 ns/loop.
PerfCount: total 1000227148 ns; loops: 916; average 1091951 ns/loop.
PerfCount: total 1000150189 ns; loops: 913; average 1095454 ns/loop.
stop
PerfCount: total 281789175 ns; loops: 258; average 1092206 ns/loop.
*/