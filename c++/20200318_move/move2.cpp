#include <iostream>

class B
{
public:
    B() : s(1)
    {
       std::cout << "B()" << std::endl;
    }
    /*
    B(const B& b) :s(b.s)
    {
       std::cout << "B&b" << std::endl;
       for (int i = 0; i < s; i++)
          ptr[i] = b.ptr[i];
    }
    */
    B(B&& b)
    {
       std::cout << "B&& b " << std::endl;
       b.s = 0;
       this->s = 1;
    }
    B& operator=(B&& b)
    {
	    std::cout << "operator(B&&)" << std::endl;
	    b.s = 0;
	    this->s = 1;
	    return *this;
    }
    B& operator=(B& b) = delete;
    B(const B& b) = delete;

    ~B()
    {
       std::cout << "~B() s=" << s << std::endl;
    }
private:
    int s;
};

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
{
  std::cout << "before move construtor f3" << std::endl;
  B b2(f3());
}
{
  std::cout << "before move operator= f3" << std::endl;
  B b3 = f3();
}
 return 0 ;
}

/*
$ ./a.out
default constructor
B&& b
~B()
~B()
*/

