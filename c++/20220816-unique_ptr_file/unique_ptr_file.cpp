#include <stdio.h>
#include <memory>

int main() {
        std::unique_ptr<FILE, decltype(&fclose)> f(fopen("unique_ptr_file.cpp", "r"), &fclose);
        std::unique_ptr<char[]> buf(new char[1024]);
        int ret = fread(buf.get(), 1, 1024, f.get());
        if (ret < 0) {
                perror("fread error");
                return -1;
        }
        printf("content: %s\n", buf.get());
        return 0;
}

/*
https://stackoverflow.com/questions/26360916/using-custom-deleter-with-unique-ptr
*/

