#pragma once

#include <stddef.h>
#include <stdint.h>

#include <vector>

const char dec2hex[]{"0123456789ABCDEF"};

// 把hex字符转成decimal数字
static inline unsigned char hex2dec(unsigned char low) {
  if (low >= '0' && low <= '9') {
    low -= '0';
  } else if (low >= 'A' && low <= 'F') {
    low = low - 'A' + 10;
  } else if (low >= 'a' && low <= 'f') {
    low = low - 'a' + 10;
  }

  return low;
}

// hex编码
inline void hex_encode(const uint8_t *bin, int binLen, char *hex, int hexLen) {
  if (hexLen < binLen * 2) {
    return;
  }

  unsigned char c;
  for (size_t i = 0; i < (size_t)binLen; i++) {
    c = bin[i];
    hex[i * 2] = dec2hex[c >> 4];
    hex[i * 2 + 1] = dec2hex[c & 0xf];
  }

  if (hexLen >= binLen * 2 + 1) {
    hex[binLen * 2] = '\0';
  }
}

// hex解码
inline void hex_decode(const char *hex, uint8_t *bin, int binLen) {
  for (size_t i = 0; i + 1 < size_t(binLen * 2 + 1); i += 2) {
    bin[i / 2] = hex2dec(hex[i]) << 4 | hex2dec(hex[i + 1]);
  }
}

// 把bin内容编码成hex，并以'\0'结尾。
inline void hexEncode(const std::vector<char> &src, std::vector<char> *dst) {
  dst->resize(src.size() * 2 + 1);
  hex_encode((uint8_t *)src.data(), src.size(), dst->data(), dst->size());
}

// 把hex内容解码成bin。
inline void hexDecode(const std::vector<char> &src, std::vector<char> *dst) {
  dst->resize(src.size() / 2);
  hex_decode(src.data(), (uint8_t *)dst->data(), dst->size());
}
