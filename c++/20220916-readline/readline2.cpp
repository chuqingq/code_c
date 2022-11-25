#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <string>

bool readline(int fd, std::string &str) {
	static char buf[1024];
	for (size_t i = 0; i < sizeof(buf); i++) {
		int ret = read(fd, buf+i, 1);
		if (ret <= 0 && i == 0) {
			return false;
		}
		if ((ret <= 0 && i != 0) || buf[i] == '\n') {
			buf[i] = '\0';
			str = std::string(buf);
			return true;
		}
	}
	buf[sizeof(buf)-1] = '\0';
	str = std::string(buf);
	return true;
}

int main() {
	int f = open("test2.cpp", O_RDONLY);
	if (f < 0) {
		return f;
	}
	std::string str;
	while (readline(f, str)) {
		printf("read: %s\n", str.c_str());
		// return 0;
	}
	return 0;
}
