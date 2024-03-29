#include <chrono>
#include <future>
#include <iostream>
#include <string>
#include <thread>

using namespace std::chrono;

std::string fetchDataFromDB(std::string recvData) {
  //确保函数要5秒才能执行完成
  std::this_thread::sleep_for(seconds(5));

  //处理创建数据库连接、获取数据等事情
  return "DB_" + recvData;
}

std::string fetchDataFromFile(std::string recvData) {
  //确保函数要5秒才能执行完成
  std::this_thread::sleep_for(seconds(5));

  //处理获取文件数据
  return "File_" + recvData;
}

int main() {
  //获取开始时间
  system_clock::time_point start = system_clock::now();

  std::future<std::string> resultFromDB =
      std::async(std::launch::async, fetchDataFromDB, "Data");

  //从文件获取数据
  std::string fileData = fetchDataFromFile("Data");

  //从DB获取数据
  //数据在future<std::string>对象中可获取之前，将一直阻塞
  std::string dbData = resultFromDB.get();

  //获取结束时间
  auto end = system_clock::now();

  auto diff = duration_cast<std::chrono::seconds>(end - start).count();
  std::cout << "Total Time taken= " << diff << "Seconds" << std::endl;

  //组装数据
  std::string data = dbData + " :: " + fileData;

  //输出组装的数据
  std::cout << "Data = " << data << std::endl;

  return 0;
}

/*
chuqq@cqqmbpr ~/t/c/c/20181002_async> ./a.out
Total Time taken= 5Seconds
Data = DB_Data :: File_Data
*/