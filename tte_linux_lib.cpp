#include <stdio.h>
#include <iostream>
#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/prettywriter.h"
#include "tte_linux_lib.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "user_config_untils/UserConfig.h"
#include "logger_untils/LoggerManager.h"

using rapidjson::Document;
using rapidjson::IStreamWrapper;
using rapidjson::SizeType;
using namespace std;
namespace hw_library
{
    namespace serial_windows
    {
        TTELINUXLib *TTELINUXLib::lib_handle_ = nullptr;

    

        TTELINUXLib::TTELINUXLib()
        {
            lib_handle_ = this;
        }


        int TTELINUXLib::SetCardID(int card_id)
        {
            cout << "开始设置板卡ID" << endl;
            card_id_ = card_id;
            return 0;
        }

        TTELINUXLib &TTELINUXLib::GetInstance()
        {
            return *lib_handle_;
        }

        

            // 解析配置文件,现在改用boost库实现，再看看cmake如何进行
            int TTELINUXLib::ParseConfig(const std::string &config_file)
        {
            std::string str;

            Config &config = Config::getInstance(config_file);

            LOG(INFO) << "My first info log using default logger";

            LOG(INFO) << "开始调用config类";
            string log_directory_ = config.getObject("user_config").get<const char *>("log_directory");

            // config.getObject("user_config");

            // string log_directory_ = config.get<const char *>("user_config.log_directory");
            LoggerManager::InitEasyloggingPP(log_directory_); 
            //   对于日志一个配置
            std::fill(read_buff_, read_buff_ + 1024, 0xFF);


            // LoggerManager::setLogEnable(el::Level::Debug, log_directory_, config.get<bool>("debug_config.debug_log_enable"));
            // LoggerManager::setLogEnable(el::Level::Error, log_directory_, config.get<bool>("user_config.error_log_enable"));
            // LoggerManager::setLogEnable(el::Level::Info, log_directory_, config.get<bool>("debug_config.info_log_enable"));

            // //   设置日志输出的路径
            // LoggerManager::setLogToStandardOutput(el::Level::Debug, config.get<bool>("debug_config.debug_log_output"));
            // LoggerManager::setLogToStandardOutput(el::Level::Error, config.get<bool>("debug_config.error_log_output"));

            // //   setLogEnable(el::Level::Info,true);
            // //   setLogToStandardOutput(el::Level::Info,true);

            // LoggerManager::setLogToFile(el::Level::Debug, config.get<bool>("debug_config.debug_log_to_file"));
            // LoggerManager::setLogToFile(el::Level::Error, config.get<bool>("debug_config.error_log_to_file"));

            SubConfig group_port = config.getObject("user_config");

            config.validateHasMember({"Channels", "user_config", "debug_config"});

            vector<SubConfig> channels = config.getArray("Channels");
            for (size_t i = 0; i < channels.size(); i++)
            {
                /* code */
                SimuHWChannel channel_info;
                channel_info.id = i;
                 snprintf(channel_info.channel, sizeof(channel_info.channel), "%s",
                                                                         channels[i].getItem<const char *>());

                snprintf(channel_info.card, sizeof(channel_info.card), "%s", "None");
                snprintf(channel_info.module, sizeof(channel_info.module), "%s",
                         "None");
                channel_config_.push_back(channel_info);
            }

            

           SubConfig user_config= config.getObject("user_config");
           SubConfig debug_config = config.getObject("debug_config");
;

            user_config.validateHasMember({"local_ip", "group_ip", "group_port", "device_1", "device_2",
                                            "log_directory", "source_directory", "error_log_enable"});


            

            debug_config.validateHasMember({"error_log_output", "debug_log_enable",
                                             "debug_log_to_file", "debug_log_output", "info_log_enable", "error_log_to_file"});


            local_ip_=user_config.get<const char *>("local_ip");
            group_ip_ = user_config.get<const char *>("group_ip");
            group_port_ = user_config.get<int>("group_port");

            


            device_1_ = user_config.getArray("device_1")[1].getItem<const char *>();
            device_2_ = user_config.getArray("device_2")[1].getItem<const char *>();

            tte_index_1_ = user_config.getArray("device_1")[0].getItem<int>();
            tte_index_2_ = user_config.getArray("device_2")[0].getItem<int>();

            log_directory_=user_config.get<const char *>("log_directory");
            source_directory_ = user_config.get<const char *>("source_directory");
            error_log_enable_ = user_config.get<bool>("error_log_enable");

            debug_log_enable_ = debug_config.get<bool>("debug_log_enable");
            debug_log_to_file_ = debug_config.get<bool>("debug_log_to_file");
            debug_log_output_ = debug_config.get<bool>("debug_log_output");
            info_log_enable_ = debug_config.get<bool>("info_log_enable");

            std::cout << "Debug Config:" << std::endl;
            std::cout << "  Error Log Enable: " << (error_log_enable_ ? "true" : "false") << std::endl;
            std::cout << "  Error Log Output: " << (error_log_output_ ? "true" : "false") << std::endl;
            std::cout << "  Debug Log Enable: " << (debug_log_enable_ ? "true" : "false") << std::endl;
            std::cout << "  Debug Log to File: " << (debug_log_to_file_ ? "true" : "false") << std::endl;
            std::cout << "  Debug Log Output: " << (debug_log_output_ ? "true" : "false") << std::endl;

            std::cout << "User Config:" << std::endl;
            std::cout << "  Local IP: " << local_ip_ << std::endl;
            std::cout << "  Group IP: " << group_ip_ << std::endl;
            std::cout << "  Group Port: " << group_port_ << std::endl;
            std::cout << "  Device 1: " << device_1_ << std::endl;
            std::cout << "  Device 2: " << device_2_ << std::endl;
            std::cout << "  Error Log to File: " << (error_log_to_file_ ? "true" : "false") << std::endl;
            std::cout << "  Log Directory: " << log_directory_ << std::endl;
            // std::cout << "  Log Filename Prefix: " << log_filename_prefix_ << std::endl;
            std::cout << "  Source Directory: " << source_directory_ << std::endl;
            return 0;
        }
        int TTELINUXLib::Init(char *config_file, char *lib_dir) {
            ParseConfig(config_file);
        };
        int TTELINUXLib::SetThreadAttr(const SimuHWThreadParam &param)
        {
            thread_param_ = param;
            return 0;
        }
        int TTELINUXLib::RegisteReadCB(SimuHWReadCB cb, void *arg)
        {
            read_cb_ = cb;
            read_cb_arg_ = arg;
            return 0;
        }

        int TTELINUXLib::SetLogCB(SimuHWLogCB cb)
        {
            return 0;
        }

        int TTELINUXLib::GetChannels(std::vector<SimuHWChannel> *channels)
        {
            assert(channels);
            channels->clear();
            for (const auto &config : channel_config_)
                channels->push_back(config);
            return 0;
        }
        int TTELINUXLib::Start(){

        }
        int TTELINUXLib::Stop()
        {
            CLOG(DEBUG, TTE_LOGGER) << "stop";

            is_run_ = false;

            CLOG(DEBUG, TTE_LOGGER) << "退出";
            printf("退出");

            return 0;
        }
        // 组包模型发给硬件模型对应的操作在这里
        int TTELINUXLib::Write(int channel_id, char *data, size_t size)
        {

            if (size > READ_BUFFER_SIZE)
            {
                return -1;
            }
            {
                std::lock_guard<std::mutex> lock(Read_buff_buffer_mutex_);
                memcpy(read_buff_, data, size);
                write_flag_ = true;
            }
            return 0;
        }
    }
}

        