#include <iostream>
#include <thread>
#include <unistd.h>
#include <string>
#include <iostream>

using namespace std;

extern string add(const string& a, const string& b);
/*
int add(int a, int b) {
	return a +b;
}
*/

extern string add2(const string& a, const string& b);
/*
int add2(int a, int b) {
	return a +b;
}
*/
int main() {
	cout << add("abc", "def") << endl;
    cout << add2("123", "456") << endl;
	return 0;
}

/*
g++ -o libtest1.so test1.cpp -fPIC -shared
g++ -o libtest2.so -static-libgcc -static-libstdc++ test2.cpp -fPIC -shared
g++ -o test -static-libgcc -static-libstdc++ test.cpp -L. -ltest1 -ltest2
export LD_LIBRARY_PATH=`pwd`

$ ./test
1+1: 2
1+2: 3
*/
