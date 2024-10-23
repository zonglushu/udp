#include "easylogging++.h"
#ifndef LoggerManager_H
#define LoggerManager_H
class LoggerManager
{
private:
    /* data */
public:
    static void InitEasyloggingPP(const std::string &absolute_path, const std::string &customLoggerName = TTE_LOGGER);
    static void setLogEnable(el::Level level, std::string &log_directory, bool enabled, const std::string &loggerName = TTE_LOGGER);
    static void setLogToFile(el::Level level, bool toFile, const std::string &loggerName = TTE_LOGGER);
    static void setLogToStandardOutput(el::Level level, bool output, const std::string &loggerName = TTE_LOGGER);
    LoggerManager(/* args */);
    ~LoggerManager();
};

#endif
