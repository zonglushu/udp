#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include "tte_linux_lib.h"
using namespace rapidjson;
#include "easylogging++.h"

#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "user_config_untils/UserConfig.h"
#include "logger_untils/LoggerManager.h"
using namespace hw_library::serial_windows;
using namespace boost::property_tree;
using namespace std;
INITIALIZE_EASYLOGGINGPP

int main()
{
    Config &config = Config::getInstance("../config.json");

    string log_directory_ = config.get<string>("user_config.log_directory");
    TTELINUXLib tte_lib;
    LoggerManager::InitEasyloggingPP(log_directory_);
    //   对于日志一个配置
    // std::fill(read_buff_, read_buff_ + 1024, 0xFF);

    LoggerManager::setLogEnable(el::Level::Debug, log_directory_, config.get<bool>("debug_config.debug_log_enable"));
    LoggerManager::setLogEnable(el::Level::Error, log_directory_, config.get<bool>("user_config.error_log_enable"));
    LoggerManager::setLogEnable(el::Level::Info, log_directory_, config.get<bool>("debug_config.info_log_enable"));

    //   设置日志输出的路径
    LoggerManager::setLogToStandardOutput(el::Level::Debug, config.get<bool>("debug_config.debug_log_output"));
    LoggerManager::setLogToStandardOutput(el::Level::Error, config.get<bool>("debug_config.error_log_output"));

    //   setLogEnable(el::Level::Info,true);
    //   setLogToStandardOutput(el::Level::Info,true);

    LoggerManager::setLogToFile(el::Level::Debug, config.get<bool>("debug_config.debug_log_to_file"));
    LoggerManager::setLogToFile(el::Level::Error, config.get<bool>("debug_config.error_log_to_file"));

    LOG(INFO) << "My first info log using default logger";

    LOG(INFO) << "开始调用config类";
    config.load("../config.json");
    config.print();
    int group_port = config.get<int>("user_config.group_port");
    LOG(INFO) << group_port;
    return 0;
}