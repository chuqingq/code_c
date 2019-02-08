// 验证mutex会轮询，导致资源浪费

#include <thread>
#include <iostream>
#include <mutex>
#include <chrono>
using namespace std::chrono_literals;

int main() {
	// 互斥量
	std::mutex m;
	
	// 线程1：加锁
	std::thread thread1([&](){
		m.lock();
		std::this_thread::sleep_for(10s);
		m.unlock();
	});
	
	std::thread thread2([&](){
		m.lock();
		std::cout << "thread2 locked" << std::endl;
		m.unlock();
	});
	
	thread1.join();
	thread2.join();
	
	return 0;
}
/*
chuqq@chuqq-matebook13 /m/d/w/t/c/c/20190208_mutex_is_poll>
g++ mutex_is_poll.cpp  -pthread
执行时发现，线程2的m.lock()并不会浪费CPU。

所以，条件变量真正节省的不是thread2的m.lock()，而是thread2不停m.lock()和m.unlock()导致的cpu浪费。
只需要thread1发现条件成熟后，通过条件变量告知thread2，thread2就能立刻lock住并进行后续处理了，无需轮询尝试。
*/