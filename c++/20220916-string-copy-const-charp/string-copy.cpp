#include <string>
#include <iostream>

using namespace std;

int main() {
	char s[] = "1234567890";
	string str(s);
	string str2(s, 9);
	s[3] = 'x';
	cout << s << ", " << str << "," << str2 << "\n";
	return 0;
}
/*
$ ./test
123x567890, 1234567890,123456789
*/

