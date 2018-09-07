#include "deps/json/include/json.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cassert>
using json = nlohmann::json;
using namespace std;

// 用find判断key是否存在，用is_string判断类型
void test1() {
    json j;
    j["name"] = 123;

    auto iter = j.find("name");
    if (iter != j.end() && iter->is_string()) {
        cout << iter->get<string>() << endl;
    }

    cout << "" << endl;
}

// 读写文件
void test2() {
    json j;
    j["name"] = 123;

    const string FILE = "1.json";
    ofstream ofs(FILE);
    assert(ofs.is_open());
    ofs << setw(4) << j;
    ofs.close();

    json i;
    ifstream ifs(FILE);
    assert(ifs.is_open());
    ifs >> i;
    ifs.close();

    cout << i << endl;
}

int main() {
    test1();
    test2();
    return 0;
}
