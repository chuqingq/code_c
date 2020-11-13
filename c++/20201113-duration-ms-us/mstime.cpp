#include <iostream>
#include <chrono>
#include <thread>

#include <sys/time.h>
#include <unistd.h>
 
void f()
{
    // std::this_thread::sleep_for(std::chrono::milliseconds(3));
	usleep(3000);
}

static long mstime() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

static unsigned long long ustime(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((unsigned long long)tv.tv_sec)*1000000 + tv.     tv_usec;           
}

int main()
{
    // auto start = mstime();
	auto start = ustime();
    f();
    // auto end = mstime();
	auto end = ustime();

    std::cout << "f() took " << end-start << " ms, " << std::endl;
}
