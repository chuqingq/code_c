#pragma once

#include <stdio.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

inline int createFileIfNotExists(std::string const &filePath) {
  std::ofstream ofs(filePath.c_str(), std::ios::out | std::ios::app);
  if (!ofs.is_open()) {
    return -1;
  }
  ofs.close();
  return 0;
}

inline int fileExists(std::string const &file) {
  std::ifstream ifs(file);
  return ifs.is_open();
}

inline int readFile(const std::string &file, std::vector<char> *data) {
  std::ifstream ifs(file, std::ios::binary);
  if (!ifs)  // 比如没有权限
  {
    return -1;
  }
  // data->assign((std::istreambuf_iterator<char>(ifs)),
  // std::istreambuf_iterator<char>());

  ifs.seekg(0, std::ios::end);
  data->resize(ifs.tellg());
  ifs.seekg(0, std::ios::beg);

  ifs.read(data->data(), data->size());

  ifs.close();
  return 0;
}

inline int writeFile(const std::string &file,
                     const std::vector<char> &content) {
  std::ofstream ofs(file, std::ios::binary);
  if (!ofs)  // 比如没有权限
  {
    return -1;
  }
  ofs.write(content.data(), content.size());
  ofs.close();
  return 0;
}

inline int deleteFile(const std::string &file) {
  //   std::string res;
  //   return executeCmd("rm -f " + file + " 2>/dev/null ", &res);
  return remove(file.c_str());
}
