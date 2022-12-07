#include "stop_watch.hpp"

#include <uv.h>
#include <iostream>

uv_async_t async1;
uv_async_t async2;

const uint64_t count = 100000;
uint64_t i = 0;

void callback(uv_async_t *handle) {
    // std::cout << "callback: " << i << std::endl;
    if (++i > count) {
        uv_close((uv_handle_t *)&async1, NULL);
        uv_close((uv_handle_t *)&async2, NULL);
    }

    uv_async_t *to;
    if (handle == &async1) {
        to = &async2;
    } else {
        to = &async1;
    }

    uv_async_send(to);
}

int main() {
    uv_async_init(uv_default_loop(), &async1, callback);
    uv_async_init(uv_default_loop(), &async2, callback);

    StopWatch time;
    uv_async_send(&async1);
    uv_run(uv_default_loop(), UV_RUN_DEFAULT); 
    time.print(count);

    return 0;
}

// g++ -o async_bench{,.cc} -Wall -luv
// StopWatch: total 27246454 ns; average 272 ns/loop.