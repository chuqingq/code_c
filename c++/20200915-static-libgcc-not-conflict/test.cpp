#include <iostream>
#include <thread>
#include <unistd.h>

using namespace std;

extern int add(int a, int b);
/*
int add(int a, int b) {
	return a +b;
}
*/

extern int add2(int a, int b);
/*
int add2(int a, int b) {
	return a +b;
}
*/
int main() {
	printf("1+1: %d\n", add(1, 1));
	printf("1+2: %d\n", add2(1, 2));
	return 0;
}

/*
g++ -o libtest1.so -static-libgcc -static-libstdc++ test1.cpp -fPIC -shared
g++ -o libtest2.so -static-libgcc -static-libstdc++ test2.cpp -fPIC -shared
g++ -o test -static-libgcc -static-libstdc++ test.cpp -L. -ltest1 -ltest2
export LD_LIBRARY_PATH=`pwd`

$ ./test
1+1: 2
1+2: 3
*/
