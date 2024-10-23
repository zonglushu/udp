#include "UserConfig.h"
#include <iostream>
#include <fstream>

#include "../rapidjson/document.h"
#include "../rapidjson/istreamwrapper.h"
#include "../rapidjson/prettywriter.h"
using namespace std;


// 静态成员初始化
once_flag Config::initFlag;
string Config::configFilename;

Config &Config::getInstance(const string &filename)
{
    configFilename = filename; // 记录文件名
    static Config instance; // 局部静态变量，线程安全
    instance.load(configFilename);
    return instance;
}



void Config::load(const string &filename)
{
    std::call_once(initFlag, [&]()
                   {
        std::ifstream ifs(filename);
        if (!ifs.is_open()) {
            throw std::runtime_error("Could not open config file: " + filename);
        }

        IStreamWrapper isw(ifs);
        document.ParseStream(isw);

        if (document.HasParseError()) {
            throw std::runtime_error("JSON parse error: " + std::to_string(document.GetParseError()) +
                                     " at " + std::to_string(document.GetErrorOffset()));
        } });
   
}

// 获取嵌套对象
SubConfig Config::getObject(const std::string &path)
{
    if (!document.HasMember(path.c_str()) || !document[path.c_str()].IsObject())
    {
        throw std::runtime_error("Key not found or not an object: " + path);
    }
    return SubConfig(document[path.c_str()]);
    // std::make_unique<SubConfig>(document[path.c_str()]); // 返回智能指针
}

vector<SubConfig> Config::getArray(const std::string &key) const
{
    if (!document.HasMember(key.c_str()) || !document[key.c_str()].IsArray())
    {
        throw std::runtime_error("Key not found or not an array: " + key);
    }

    std::vector<SubConfig> result;
    const Value &array = document[key.c_str()];
    
    for (SizeType i = 0; i < array.Size(); ++i)
    {
        const Value &element = array[i]; // 获取引用

        result.push_back(SubConfig(element)); // 使用智能指针
    }
    return result;
}

void Config::validateHasMember(initializer_list<string> keys)
{
    for (const auto &key : keys)
    {
        if (!document.HasMember(key.c_str()))
        {
            throw std::runtime_error("Missing required key: " + key);
        }
    }
}


void Config::print() const
{
    for (auto &m : document.GetObject())
    {
        std::cout << m.name.GetString() << ": " << m.value.GetString() << std::endl;
    }
}