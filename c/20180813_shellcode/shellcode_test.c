  int main(int argc, char **argv) {
      int (*f)() = (int(*)()) argv[1];
      return (*f)();
  }

