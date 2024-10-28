#include "UserConfig.h"
#include "DeviceConfig.h"
#include <iostream>
#include <unordered_map>
#include <memory>
#include <stdexcept>

using namespace utils::deviceconfig;
// 配置管理器
class ConfigManager
{
public:
    static ConfigManager &getInstance()
    {
        static ConfigManager instance;
        return instance;
    }

    // 获取用户配置
    UserConfig &getUserConfig()
    {
        if (!userConfigLoaded)
        {
            userConfig = std::make_unique<UserConfig>();
            userConfig->load("user_config.json"); // 加载用户配置文件
            userConfigLoaded = true;
        }
        return *userConfig;
    }

    // 获取设备配置，按需加载
    DeviceConfig &getDeviceConfig(const std::string &deviceId)
    {
        if (deviceConfigMap.find(deviceId) == deviceConfigMap.end())
        {
            deviceConfigMap[deviceId] = std::make_unique<DeviceConfig>(deviceId);
        }
        return *deviceConfigMap[deviceId];
    }

private:
    ConfigManager() = default;
    std::unique_ptr<UserConfig> userConfig;
    std::unordered_map<std::string, std::unique_ptr<DeviceConfig>> deviceConfigMap;
    bool userConfigLoaded = false;
};