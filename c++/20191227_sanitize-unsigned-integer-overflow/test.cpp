#include <string>
#include <string_view>
#include <type_traits>

#include <memory>
#include <map>
#include <iostream>

using namespace std;
 
int main()
{
	unsigned a = 0;
	unsigned b = a - 1;
	cout << b << endl;
	return 0;
}

/*
➜  temp clang++ test.cpp  -Wall -fsanitize=unsigned-integer-overflow
➜  temp ./a.out
test.cpp:14:17: runtime error: unsigned integer overflow: 0 - 1 cannot be represented in type 'unsigned int'
4294967295
*/

