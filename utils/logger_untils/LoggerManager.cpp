#include "LoggerManager.h"

#include "user_config_untils/UserConfig.h"

INITIALIZE_EASYLOGGINGPP
void LoggerManager::InitEasyloggingPP(const std::string &absolute_path, const std::string &customLoggerName)
{
    // 添加严格的日志文件大小检查标志
    el::Loggers::addFlag(el::LoggingFlag::StrictLogFileSizeCheck);

    el::Configurations conf;

    // 设置默认配置
    conf.set(el::Level::Global, el::ConfigurationType::Enabled, "false"); // Disable all by default
    conf.set(el::Level::Global, el::ConfigurationType::ToStandardOutput, "false");
    conf.set(el::Level::Global, el::ConfigurationType::Format, "[%level | %datetime] | %msg");
    conf.set(el::Level::Global, el::ConfigurationType::MillisecondsWidth, "3");
    conf.set(el::Level::Global, el::ConfigurationType::PerformanceTracking, "false");
    conf.set(el::Level::Global, el::ConfigurationType::MaxLogFileSize, "10485760"); // 1 MB
    conf.set(el::Level::Global, el::ConfigurationType::LogFlushThreshold, "0");

    // Enable only debug and error levels
    conf.set(el::Level::Debug, el::ConfigurationType::Enabled, "false");
    conf.set(el::Level::Debug, el::ConfigurationType::ToFile, "false"); // Debug not to file by default
    // conf.set(el::Level::Debug, el::ConfigurationType::Filename, absolute_path+"/debug_log_%datetime{%Y_%M_%d_%H_%m_%s}.log");

    conf.set(el::Level::Error, el::ConfigurationType::Enabled, "true");
    conf.set(el::Level::Error, el::ConfigurationType::ToFile, "true"); // Error to file by default
    // conf.set(el::Level::Error, el::ConfigurationType::Filename, absolute_path+"/error_log_%datetime{%Y_%M_%d_%H_%m_%s}.log");

    conf.set(el::Level::Info, el::ConfigurationType::Enabled, "false");
    conf.set(el::Level::Info, el::ConfigurationType::ToFile, "false"); // Error to file by default
    conf.set(el::Level::Info, el::ConfigurationType::ToStandardOutput, "true");
    // 重新配置所有记录器
    el::Loggers::reconfigureAllLoggers(conf);
    // 注册自定义记录器
    el::Logger *customLogger = el::Loggers::getLogger(customLoggerName);
    // 重新配置自定义记录器
    el::Loggers::reconfigureLogger(customLoggerName, conf);
}

void LoggerManager::setLogEnable(el::Level level, std::string &log_directory, bool enabled, const std::string &loggerName )
{
    UserConfig &config = UserConfig::getInstance("../config.json");
    bool error_log_to_file_ = config.get<bool>("debug_config.error_log_to_file");
    bool debug_log_to_file_ = config.get<bool>("debug_config.debug_log_to_file");

    el::Configurations conf;
    if (enabled && level == el::Level::Error && error_log_to_file_)
    {
        conf.set(level, el::ConfigurationType::Filename, log_directory + "/error_log_%datetime{%Y_%M_%d_%H_%m_%s}.log");
    }
    else if (enabled && level == el::Level::Debug && debug_log_to_file_)
    {
        conf.set(level, el::ConfigurationType::Filename, log_directory + "/debug_log_%datetime{%Y_%M_%d_%H_%m_%s}.log");
    }
    conf.set(level, el::ConfigurationType::Enabled, enabled ? "true" : "false");
    el::Loggers::reconfigureLogger(loggerName, conf);
}

void LoggerManager::setLogToFile(el::Level level, bool toFile, const std::string &loggerName )
{
    el::Configurations conf;
    conf.set(level, el::ConfigurationType::ToFile, toFile ? "true" : "false");
    el::Loggers::reconfigureLogger(loggerName, conf);
}

void LoggerManager::setLogToStandardOutput(el::Level level, bool output, const std::string &loggerName )
{
    el::Configurations conf;
    conf.set(level, el::ConfigurationType::ToStandardOutput, output ? "true" : "false");
    el::Loggers::reconfigureLogger(loggerName, conf);
}
