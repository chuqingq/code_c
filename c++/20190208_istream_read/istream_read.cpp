// read a file into memory
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream

int main () {

  std::ifstream is ("test.txt", std::ifstream::binary);
  if (is) {
    // get length of file:
    is.seekg (0, is.end);
    int length = is.tellg();
	std::cout << "file length: " << length << std::endl;
    is.seekg (0, is.beg);

	length = 3; // 因为每次read的第三个参数都是8191，所以缩小分配的内存试试。改过之后底层还是8191。。。
    char * buffer = new char [length];

    std::cout << "Reading " << length << " characters... ";
    // read data as a block:
    is.read (buffer,length);

    if (is) // 这里有问题，如果读少了（比如文件6B，只读了3B），is还是true
      std::cout << "all characters read successfully.";
    else
      std::cout << "error: only " << is.gcount() << " could be read";
    is.close();

    // ...buffer contains the entire file...

    delete[] buffer;
  }
  return 0;
}
/*
chuqq@chuqq-matebook13 /m/d/w/t/c/c/20190208_istream_read>
g++ istream_read.cpp
chuqq@chuqq-matebook13 /m/d/w/t/c/c/20190208_istream_read> ./a.out
file length: 6
Reading 3 characters... all characters read successfully.⏎                     chuqq@chuqq-matebook13 /m/d/w/t/c/c/20190208_istream_read>
strace ./a.out 2>&1 |grep read
read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\360\303\10\0\0\0\0\0"..., 832) = 832
read(3, "\177ELF\2\1\1\0\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\300*\0\0\0\0\0\0"..., 832) = 832
read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\260\34\2\0\0\0\0\0"..., 832) = 832
read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\200\272\0\0\0\0\0\0"..., 832) = 832
read(3, "123456", 8191)                 = 6
write(1, "Reading 3 characters... all char"..., 57Reading 3 characters... all characters read successfully.) = 57
⏎

主要两个问题：
1、确定：ifstream.read最终调用系统调用read
2、问题：最终系统调用的read的第二个参数大小是8191，并不是实际分配的内存大小，猜测是c++编译器优化过的

*/