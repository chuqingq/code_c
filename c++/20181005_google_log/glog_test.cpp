// g++ glog_test.cpp -lglog
// g++ glog_test.cpp -lglog -Wall -lgflags
#include <glog/logging.h>
 
int main(int argc,char* argv[]) {
    // If glog is used to parse the command line 
    google::ParseCommandLineFlags(&argc, &argv, true);
 
    // Initialize Google's logging library.
    google::InitGoogleLogging(argv[0]);
 
    // FLAGS_log_dir = "./log";
 
    LOG(INFO) << "hello world";
    LOG(ERROR) << "this is an error log";
    LOG(WARNING) << "this is a warn log";
    // LOG(FATAL) << "this is a fatal log";
 
    return 0;
}
/*
INFO, WARNING, ERROR, FATAL、分别对应数字
0, 1, 2, 3

使用参数有三种方式：
1、环境变量，例如
    export GLOG_log_dir=log
    export GLOG_minloglevel=1
    export GLOG_stderrthreshold=1
    export GLOG_v=3
    export GLOG_max_log_size=1
2、命令行参数： ./a.out --log_dir=log --minloglevel=1
3、直接在代码中修改FLAGS_log_dir变量
*/
