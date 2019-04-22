// 下载json.hpp
// wget -c https://github.com/nlohmann/json/releases/download/v3.6.1/json.hpp

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cassert>

using namespace std;

#include "json.hpp"
using json = nlohmann::json;

// 用find判断key是否存在，用is_string判断类型
void test1()
{
    json j;
    j["name"] = 123;

    auto iter = j.find("name");
    if (iter != j.end() && iter->is_string())
    {
        cout << iter->get<string>() << endl;
    }

    cout << "" << endl;
}

// 读写文件
void test2()
{
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

// 字符串
void test3()
{
    json j = json::parse("{\"name\":\"value\", \"name1\":\"value1\"}");
    cout << j["name"].get<std::string>() << endl;

    // j = {{"name", "value"}, {"name1", "value1"}};

    cout << "dump: " << j.dump(0) << endl;
    cout << "dump: " << j.dump(1) << endl;
}

void test4()
{
    json j = {"name1", "nam2"};
    for (auto const &i : j)
    {
        cout << i << endl;
    }
}

int main()
{
    test1();
    test2();
    test3();
    test4();
    return 0;
}
