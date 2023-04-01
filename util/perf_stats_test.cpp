#include "perf_stats.hpp"
#include <chrono>

int main() {
    PerfStats p;
    for (int i = 0; i < 3000; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        p.Incr();
    }
    std::cout << "stop\n";
    p.Stop();
    return 0;
}
/*
$ g++ -o perf_stats_test{,.cpp} -g -Wall
$ ./perf_stats_test
PerfCount[unknown]: total 1000447457 ns; loops: 772; average 1295916 ns/loop.
PerfCount[unknown]: total 1001069958 ns; loops: 717; average 1396192 ns/loop.
PerfCount[unknown]: total 1000506457 ns; loops: 705; average 1419158 ns/loop.
PerfCount[unknown]: total 1000574458 ns; loops: 718; average 1393557 ns/loop.
stop
PerfCount[unknown]: total 107790143 ns; loops: 88; average 1224887 ns/loop.
*/