#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <iterator>

using namespace std;

int readFileTo(const std::string &file, std::vector<char> *data)
{
    std::ifstream ifs(file, std::ios::binary);
    if (!ifs) // 比如没有权限
    {
        return -1;
    }
    data->assign((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    ifs.close();

    return 0;
}

int writeFile(const std::string &file, const std::vector<char> &content)
{
    std::ofstream ofs(file, std::ios::binary);
    if (!ofs) // 比如没有权限
    {
        return -1;
    }
    ofs.write(content.data(), content.size());
    ofs.close();
    return 0;
}

int main()
{
    const std::string file("/home/chuqq/.bashrc");

    std::vector<char> content;
    readFileTo("/home/chuqq/.bashrc", &content);
    cout << content.size() << endl;

    writeFile(file + ".bak", content);
    return 0;
}
