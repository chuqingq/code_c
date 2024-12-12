#include <bits/stdc++.h>
using namespace std;

const int kCount = 10000000;

int add(int a, int b) {
  return a + b;
}

// 使用std::bind和std::function的测试函数
void bindAndFunctionCall(std::function<int(int)> &call) {
  for (int i = 0; i < kCount; ++i) {
    call(i);
  }
}

// 使用lambda表达式的测试函数
void lambdaCall() {
  auto lambda_add = [](int b) {
    return add(10, b);
  };
  for (int i = 0; i < kCount; ++i) {
    lambda_add(i);
  }
}


// 使用lambda表达式的测试函数
void lambdafunctionCall(std::function<int(int)> &func) {
  for (int i = 0; i < kCount; ++i) {
    func(i);
  }
}


template <typename T>

void lambdatemplateCall(T &func) {
  for (int i = 0; i < kCount; ++i) {
    func(i);
  }
}

int main() {
  using clock = std::chrono::high_resolution_clock;

  {
    auto start = clock::now();
    auto bound_add = std::bind(add, 10, std::placeholders::_1);
    std::function<int(int)> func = bound_add;
    bindAndFunctionCall(func);
    auto end = clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Bind and function call time: " << diff.count() << " s" << std::endl;
  }

  {
    auto start = clock::now();
    lambdaCall();
    auto end = clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Lambda call time: " << diff.count() << " s" << std::endl;
  }

  {
    auto start = clock::now();
    auto lambda_add = [](int b) {
      return add(10, b);
    };
    std::function<int(int)> func = lambda_add;
    lambdafunctionCall(func);
    auto end = clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Lambdafunction call time: " << diff.count() << " s" << std::endl;
  }

  {
    auto start = clock::now();
    auto lambda_add = [](int b) {
      return add(10, b);
    };
    lambdatemplateCall(lambda_add);
    auto end = clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Lambdatemplate call time: " << diff.count() << " s" << std::endl;
  }

  return 0;
}

//Bind and function call time: 0.598768 s
//Lambda call time: 0.0222757 s
//Lambdafunction call time: 0.203756 s
//Lambdatemplate call time: 0.0253566 s