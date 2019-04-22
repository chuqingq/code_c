#include <vector>
#include <iostream>
#include <stdint.h>

using namespace std;

const char dec2hex[]{"0123456789ABCDEF"};

// 把bin内容编码成hex，并以'\0'结尾。
void hexEncode(const std::vector<char> &src, std::vector<char> *dst)
{
    dst->resize(src.size() * 2 + 1);

    unsigned char c;
    for (size_t i = 0; i < src.size(); i++)
    {
        c = src[i];
        (*dst)[i * 2] = dec2hex[c >> 4];
        // cout << (*dst)[i * 2];
        (*dst)[i * 2 + 1] = dec2hex[c & 0xf];
        // cout << (*dst)[i * 2 + 1];
    }
    // cout << endl;

    (*dst)[src.size() * 2 + 1] = '\0';
}

// 把hex字符转成decimal数字
static inline unsigned char hex2dec(unsigned char low)
{
    if (low >= '0' && low <= '9')
    {
        low -= '0';
    }
    else if (low >= 'A' && low <= 'F')
    {
        low = low - 'A' + 10;
    }
    else if (low >= 'a' && low <= 'f')
    {
        low = low - 'a' + 10;
    }

    return low;
}

// 把hex内容解码成bin。
void hexDecode(const std::vector<char> &src, std::vector<char> *dst)
{
    dst->resize(src.size() / 2);

    for (size_t i = 0; i + 1 < src.size(); i += 2)
    {
        // cout << "src: " << (int)hex2dec(src[i]) << " " << (int)hex2dec(src[i + 1]) << endl;
        (*dst)[i / 2] = hex2dec(src[i]) << 4 | hex2dec(src[i + 1]);
        // cout << "dst: " << (int)(*dst)[i / 2] << " " << endl;
    }
    // cout << endl;
}

void hexCodecTest(std::vector<char> const &d, std::vector<char> const &h)
{
    std::vector<char> t;

    hexEncode(d, &t);
    if (t != h)
    {
        cout << "error: " << t.data() << ", should be " << h.data() << endl;
    }

    hexDecode(h, &t);
    if (t != d)
    {
        cout << "error2" << endl;
    }
}

int main()
{
    std::vector<char> d{1, 18, 10, 27, -1};
    std::vector<char> h{'0', '1', '1', '2', '0', 'A', '1', 'B', 'F', 'F', '\0'};
    hexCodecTest(d, h);

    std::vector<char> d1{1, 2, 3, 127, -100};
    std::vector<char> h1{'0', '1', '0', '2', '0', '3', '7', 'F', '9', 'C', '\0'};
    hexCodecTest(d1, h1);
    return 0;
}
