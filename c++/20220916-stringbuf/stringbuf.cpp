#include <iostream>
#include <sstream>
#include <thread>

#include <unistd.h>

using namespace std;

stringbuf buf("", ios_base::in | ios_base::out | ios_base::app);

// produce
void f1() {
	for (int i = 0; i < 10000; i++) {
		buf.sputc('0' + (i%10));
		if ((i%10) == 9) {
			buf.sputc('\n');
		}
		usleep(100);
	}
}

// consume
void f2() {
	// istream is(&buf);
	// string str;
	while (1) {
		// str = "";
		// if (is.eof()) {
		// 	printf("consume eof\n");
		// }
		// getline(is, str);
		cout << "consume: " << &buf << "\n";
		// cout << "consume: " << str << "\n";
		usleep(10000);
	}
}

int main() {
	thread t1(f1);
	thread t2(f2);
	t1.join();
	t2.join();
	return 0;
}

