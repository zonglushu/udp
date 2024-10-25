#ifndef SubUserConfig_H
#define SubUserConfig_H
#include <vector>
#include <memory>
#include <string>
#include <stdexcept>
#include <iostream>

#include <functional>
#include "../rapidjson/document.h"
#include "../rapidjson/istreamwrapper.h"
#include "../rapidjson/prettywriter.h"
using namespace std;
using namespace rapidjson;

class SubUserConfig
{
public:
    SubUserConfig(const rapidjson::Value &value) : value_(value) {}

    
    template <typename T>
    T get(const std::string &key) const
    {
        if (!value_.HasMember(key.c_str()))
        {
            throw std::runtime_error("Key not found: " + key);
        }
        const rapidjson::Value &item = value_[key.c_str()];
        if (!item.Is<T>())
        {
            throw std::runtime_error("Type mismatch for key: " + key);
        }
        return item.Get<T>();
    }


    template <typename T>
    T getItem() const
    {
        if (!value_.IsObject())
        {
            return value_.Get<T>();
        }
        // return value_.Get<T>();
    }

    // 获取嵌套对象，返回智能指针
    SubUserConfig getObject(const std::string &path)
    {
        if (!value_.HasMember(path.c_str()) || !value_[path.c_str()].IsObject())
        {
            throw std::runtime_error("Key not found or not an object: " + path);
        }
        return  SubUserConfig(value_[path.c_str()]);
        // std::make_unique<SubUserConfig>(document[path.c_str()]); // 返回智能指针
    }

    vector<SubUserConfig> getArray(const std::string &key) const
    {
        if (!value_.HasMember(key.c_str()) || !value_[key.c_str()].IsArray())
        {
            throw std::runtime_error("Key not found or not an array: " + key);
        }

        std::vector<SubUserConfig> result;
        const Value &array = value_[key.c_str()];
        
        for (SizeType i = 0; i < array.Size(); ++i)
        {
            const Value &element = array[i]; // 获取引用
            result.push_back(SubUserConfig(element)); // 使用智能指针
        }
        return result;
    }

    void validateHasMember(initializer_list<string> keys)
    {
        for (const auto &key : keys)
        {
            if (!value_.HasMember(key.c_str()))
            {
                throw std::runtime_error("Missing required key: " + key);
            }
        }
    }

private:
    const rapidjson::Value &value_; // 持有嵌套对象的引用
};
#endif