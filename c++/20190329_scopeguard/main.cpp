#include <ScopeGuard.h>

#include <stdio.h>

int main()
{
	{
		int a = 123;
		ScopeGuard sg([&]() { printf("a111 = %d\n", a); });
		ON_SCOPE_EXIT([&]() { printf("a1112 = %d\n", a); });
		// printf("%s\n", SCOPEGUARD_LINENAME(EXIT, __LINE__));
		a = 456;
	}

	try
	{
		int a = 123;
		ScopeGuard sg2([&]() { printf("a222 = %d\n", a); });
		a = 456;
		throw 1;
	}
	catch (...)
	{
	}

	{
		int a = 123;
		ScopeGuard sg1([&]() { printf("a333 = %d\n", a); });
		a = 456;
		sg1.Dismiss();
	}

	return 0;
}
