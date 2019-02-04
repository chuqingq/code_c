// rvo: return value optimization
// 返回值优化：消除返回值产生的临时对象

#include <iostream> 
using namespace std;

class A {
public:
	int a;
	
	// 构造函数
	A(int a) { 
		this->a = a;
		cout << "A(int): " << this->a << endl;
	}

	// 拷贝构造函数
	A(const A& a) {
		this->a = a.a + 1;
		cout << "A(A&): " << this->a << endl;
	}

	// 析构函数
	~A() {
		cout << "~A(): " << this->a << endl;
	}
};

static A test(int a) {
	return A(a);
}

int main() {
	A a = test(10);
	return 0;
}
/*
默认情况（RVO）：
chuqq@cqqmbpr /V/w/t/c/c/20190204_rvo> g++ test_rvo.cpp
chuqq@cqqmbpr /V/w/t/c/c/20190204_rvo> ./a.out
A(int): 10
~A(): 10

不RVO的情况：
chuqq@cqqmbpr /V/w/t/c/c/20190204_rvo> g++ test_rvo.cpp -fno-elide-constructors
chuqq@cqqmbpr /V/w/t/c/c/20190204_rvo> ./a.out
A(int): 10
A(A&): 11
~A(): 10
A(A&): 12
~A(): 11
~A(): 12

*/