#include <iostream>
using namespace std;

template<int number, int i>
struct Prime2 {
	static const bool isPrime = (i==1)?false:((number%i)?Prime2<number, i-1>::isPrime:false);
};
template<int number>
struct Prime2<number, 1> {
	static const bool isPrime = true;
};

template<int number>
struct Prime {
	static const bool isPrime = Prime2<number, number-1>::isPrime;
};

int main() {
	cout << "2: " << Prime<2>::isPrime << endl;
	cout << "3: " << Prime<3>::isPrime << endl;
	cout << "4: " << Prime<4>::isPrime << endl;
        cout << "5: " << Prime<5>::isPrime << endl;
        cout << "6: " << Prime<6>::isPrime << endl;
	return 0;
}
/*
chuqq@chuqq-hp:~/temp$ ./a.out 
2: 1
3: 1
4: 0
5: 1
6: 0
*/
