#include <memory>
#include <stdlib.h>

int main() {
  {
    std::shared_ptr<char> a((char*)malloc(10), free);
    (void)a;
  }

    return 0;
}
