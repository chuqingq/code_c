#include <array>
#include <memory>
#include <iostream>
#include <thread>

void func()
{
	std::unique_ptr<std::array<int, 3>> a(new std::array<int, 3>{1, 2, 3});
	for (auto it = a->begin(); it != a->end(); it++)
	{
		std::cout << *it << std::endl;
	}
}

int main()
{
	std::thread t1(func);
	t1.join();
	return 0;
}

/*
在新版本的环境上：
默认：
> g++ -o sample2_not_static sample2.cpp -pthread
添加-static-libxxx选项：
> g++ -o sample2_static sample2.cpp -static-libstdc++ -static-libgcc

在旧版本的环境上（ubuntu14.04）
# ldd sample2_*
sample2_not_static:
./sample2_not_static: /usr/lib/x86_64-linux-gnu/libstdc++.so.6: version `CXXABI_1.3.9' not found (required by ./sample2_not_static)
./sample2_not_static: /usr/lib/x86_64-linux-gnu/libstdc++.so.6: version `GLIBCXX_3.4.22' not found (required by ./sample2_not_static)
	linux-vdso.so.1 =>  (0x00007fff6af62000)
	libstdc++.so.6 => /usr/lib/x86_64-linux-gnu/libstdc++.so.6 (0x00007f2da38d9000)
	libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007f2da35d3000)
	libgcc_s.so.1 => /lib/x86_64-linux-gnu/libgcc_s.so.1 (0x00007f2da33bd000)
	libpthread.so.0 => /lib/x86_64-linux-gnu/libpthread.so.0 (0x00007f2da319f000)
	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f2da2dda000)
	/lib64/ld-linux-x86-64.so.2 (0x00007f2da3bdd000)
sample2_static:
	linux-vdso.so.1 =>  (0x00007ffc45b4c000)
	libpthread.so.0 => /lib/x86_64-linux-gnu/libpthread.so.0 (0x00007f7fcd8a0000)
	libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007f7fcd59a000)
	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f7fcd1d5000)
	/lib64/ld-linux-x86-64.so.2 (0x00007f7fcdabe000)
*/
