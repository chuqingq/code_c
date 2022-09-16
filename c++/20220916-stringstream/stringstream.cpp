#include <iostream>
#include <sstream>

using namespace std;

int main()
{
    // std::stringstream buf("test\n123", std::ios_base::ate);
    std::stringstream buf("test\n123");
    string str;

    getline(buf, str);
    cout << str << "\n";

    getline(buf, str);
    cout << str << "\n";

    return 0;
}
