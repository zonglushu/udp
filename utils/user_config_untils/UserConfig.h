#ifndef CONFIG_H
#define CONFIG_H

#include <mutex>
#include "SubUserConfig.h"
#include <functional>
#include "../rapidjson/document.h"
#include "../rapidjson/istreamwrapper.h"
#include "../rapidjson/prettywriter.h"
using namespace rapidjson;


using namespace std;
class Config
{
public:
    static Config &getInstance(const string &filename); // 获取单例实例
    void load(const string &filename); // 从文件加载配置

    template <typename T>
    T get(const string &path) const; // 获取配置项的值
    void print() const;                   // 打印所有配置项
    void validateHasMember(initializer_list<string> keys); //校验多项属性是否存在
    SubConfig getObject(const std::string &path); // 获取嵌套对象
    vector<SubConfig> getArray(const std::string &key) const;

private:
    Document document;           // RapidJSON 文档
    static once_flag initFlag;   // 初始化标记
    static string configFilename; // 配置文件名
};


//供第一级调用
template <typename T>
T Config::get(const std::string &path) const
{
    if (!document.HasMember(path.c_str()))
    {
        throw std::runtime_error("Key not found: " + path);
    }

    const Value &value = document[path.c_str()];
    


    T result = value.Get<T>();
    return result;
}



#endif // CONFIG_H

