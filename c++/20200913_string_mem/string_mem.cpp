#include <iostream>
#include <string>
#include <string.h>

using namespace std;
int main() {
    string a;
    cout << "size: " << a.size() << endl;
    // 分配内存空间
    a.resize(100);
    cout << "new size: " << a.size() << endl;
    // 写入
    // a.assign("1234567890\0", 11); // 会更改string.size()
    memmove((void*)a.data(), "1234567890\0", 11);
	cout << "new new size: " << a.size() << endl;
    cout << "input: " << a << "====" << endl;
    // 取出
    char buf[11];
    a.copy(buf, sizeof(buf));
    cout << "output: " << buf << "====" << endl;
	return 0;
}

/*
size: 0
new size: 100
new new size: 100
input: 1234567890====
output: 1234567890====
*/
