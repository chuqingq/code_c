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
*/
