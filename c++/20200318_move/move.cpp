#include <iostream>

class B
{
public:
    B() :s(10), ptr(new int[s])
    {
       std::cout << "default constructor" << std::endl;
       for (int i = 0; i < s; i++)
       {
          ptr[i] = i;
       }
    }
    B(const B& b) :s(b.s)
    {
       std::cout << "B&b" << std::endl;
       for (int i = 0; i < s; i++)
          ptr[i] = b.ptr[i];
    }
    B(B&& b) :s(b.s), ptr(b.ptr)
    {
       std::cout << "B&& b" << std::endl;
       b.ptr = 0;
    }
    ~B()
    {
       std::cout << "~B()" << std::endl;
       if (ptr)
          delete[] ptr;
    }
    friend  std::ostream& operator <<(std::ostream& os, B& b);
private:
    int s;
    int* ptr;
};
std::ostream& operator <<(std::ostream& os,B& b)
{
    os << "[ ";
    for (int i = 0; i < b.s; i++)
       os << b.ptr[i] << " ";
    os << "]" << std::endl;
    return os;
}

B&& f()//注意这里B&&
{
    B tmp;
    return std::move(tmp); // warning: reference to local variable ‘tmp’ returned [-Wreturn-local-addr]
}



B& f2()//注意这里的B&
{
    B t;
    return t; // warning: reference to local variable ‘tmp’ returned [-Wreturn-local-addr]
}

B f3()
{
	B t;
	return std::move(t);
}

int main()
{
 // B b0(f2());
 // B b1(f());
 B b2(f3());
 return 0 ;
}

/*
$ ./a.out
default constructor
B&& b
~B()
~B()
*/

