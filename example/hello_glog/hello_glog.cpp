#include <iostream>
#include "glog/logging.h"
#include "root_directory.h"

static void InitLog(const char* cmd, const char* log_dir)
{
    FLAGS_logbufsecs = 0;
    google::InitGoogleLogging(cmd);
    FLAGS_log_dir = log_dir;
}

int main(int argc, char** argv)
{
    std::string log_dir = PaimonRoot + "/example/hello_glog/log";
    InitLog(argv[0], log_dir.c_str());

    LOG(INFO) << "Hello world glog";
    std::cout << "hello world" << std::endl;
    return 0;
}