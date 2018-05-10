#include <memory>
#include <functional>
#include <stdlib.h>

int main() {
  {
    std::shared_ptr<char> a((char*)malloc(10), free);
    (void)a;
  }

  {
    std::unique_ptr<char, std::function<void(char *)> > b((char*)malloc(10), free);
    (void)b;
  }

    return 0;
}
