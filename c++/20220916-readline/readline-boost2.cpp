#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/categories.hpp>

using namespace boost;
namespace io = boost::iostreams;

class my_source {
public:
	typedef char char_type;
	struct category: io::source_tag, io::closable_tag {};
	my_source(const char* file) {
		std::cout << "my_source()\n";
		fd_ = ::open(file, O_RDONLY);
	}
	/*
	my_source(const my_source& inst) {
		printf("my_source(const my_source&)\n");
		fd_ = inst.fd_;
	}
	*/
	~my_source() {
		printf("~my_source()\n");
	}
	std::streamsize read(char_type* s, std::streamsize n) {
		return ::read(fd_, s, n);
	}
	void close() {
		printf("close()\n");
		::close(fd_);
	}
private:
	int fd_;
};

int main() {
	io::stream<my_source> in("readline-boost.cpp");
	for (std::string line; getline(in, line); ) {
		std::cout << "line: " << line << std::endl;
	}
	return 0;
}
