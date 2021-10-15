#include <unistd.h>
#include <dlfcn.h>

typedef void (*fptr)();

int main() {
	void *h1 = dlopen("./libmy1.so", RTLD_LAZY);
	fptr p1 = (fptr)dlsym(h1, "myprint");
	p1();

	void *h2 = dlopen("./libmy2.so", RTLD_LAZY);
	fptr p2 = (fptr)dlsym(h2, "myprint");
	p2();

	return 0;
}
