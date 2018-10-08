#include <iostream>
#include <memory>

void myfunc(std::unique_ptr<int> b) {
	std::cout << "myfunc: " << *b.get() << std::endl;
}

int main() {
	std::unique_ptr<int> a(new int(10));
	std::cout << "before myfunc: " << *a.get() << std::endl;
	myfunc(std::move(a));
	std::cout << "after myfunc: " << *a.get() << std::endl;
	return 0;
}
/*
如果不使用move，则报错：
chuqq@chuqq-hp:~/temp/code_c/c++/20181003_make_unique$ clang++ unique_pass.cpp -std=c++14
unique_pass.cpp:11:9: error: call to deleted constructor of 'std::unique_ptr<int>'
        myfunc(a);
               ^
/usr/bin/../lib/gcc/x86_64-linux-gnu/7.3.0/../../../../include/c++/7.3.0/bits/unique_ptr.h:388:7: note: 
      'unique_ptr' has been explicitly marked deleted here
      unique_ptr(const unique_ptr&) = delete;
      ^
unique_pass.cpp:4:34: note: passing argument to parameter 'b' here
void myfunc(std::unique_ptr<int> b) {
                                 ^
1 error generated.

如果使用move，则：
chuqq@chuqq-hp:~/temp/code_c/c++/20181003_make_unique$ ./a.out 
before myfunc: 10
myfunc: 10
段错误 (核心已转储)

*/
