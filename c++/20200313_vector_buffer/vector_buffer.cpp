#include <vector>
#include <string>
#include <iostream>

using namespace std;

int main() {
	vector<char> v;
	cout << "v.size() = " << v.size() << ", cap = " << v.capacity() << endl;
	v.resize(10);
	cout << "v.size() = " << v.size() << ", cap = " << v.capacity() << endl;

	string a("123");
	v.assign(a.begin(), a.end());
	cout << "v.size() = " << v.size() << ", cap = " << v.capacity() << endl;

	return 0;
}
