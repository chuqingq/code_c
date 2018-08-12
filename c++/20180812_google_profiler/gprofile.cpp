#include <unistd.h>

#include <gperftools/profiler.h>

int a = 0;

void myfunc() {
	// sleep(3);
	for (int i = 0; i < 20000; i++) {
		a++;
	}
}

int main() {
	ProfilerStart("123");
	for (int i = 0; i < 20000; i++) {
		myfunc();
	}
	ProfilerStop();
	return 0;
}

/*
 * apt search perftool
 * sudo apt install google-perftools libgoogle-perftools-dev
 * g++ gprofile.cpp -lprofiler
 * ./a.out # 生成123 prof文件
 * google-pprof --gv ./a.out 123 # 打开文件
 */

