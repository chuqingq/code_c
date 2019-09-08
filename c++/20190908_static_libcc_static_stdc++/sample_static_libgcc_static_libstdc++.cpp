#include <array>
#include <memory>
#include <iostream>

int main()
{
    // auto b = new int[3]{1, 2, 3};
    std::unique_ptr<std::array<int, 3>> a(new std::array<int, 3>{1, 2, 3});
    for (auto it = a->begin(); it != a->end(); it++)
    {
        std::cout << *it << std::endl;
    }
    return 0;
}

/*
在新版本的环境上：
默认：
> g++ -o sample_not_static sample_static_libgcc_static_libstdc++.cpp 
添加-static-libxxx选项：
> g++ -o sample_static sample_static_libgcc_static_libstdc++.cpp -static-libstdc++ -static-libgcc

在旧版本的环境上（ubuntu14.04）
# ldd a.out sample_static 
a.out:
./a.out: /usr/lib/x86_64-linux-gnu/libstdc++.so.6: version `CXXABI_1.3.9' not found (required by ./a.out)
	linux-vdso.so.1 =>  (0x00007ffe6714c000)
	libstdc++.so.6 => /usr/lib/x86_64-linux-gnu/libstdc++.so.6 (0x00007f8891450000)
	libgcc_s.so.1 => /lib/x86_64-linux-gnu/libgcc_s.so.1 (0x00007f889123a000)
	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f8890e75000)
	libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007f8890b6f000)
	/lib64/ld-linux-x86-64.so.2 (0x00007f8891957000)
sample_static:
	linux-vdso.so.1 =>  (0x00007ffcd33e0000)
	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f446ae19000)
	/lib64/ld-linux-x86-64.so.2 (0x00007f446b4c5000)
*/
