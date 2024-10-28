#include "DeviceConfig.h"

#define PORTFILEPREFIX "stream"
static once_flag initFlag;    // 初始化标记
static string configFilename; // 配置文件名


using namespace utils::deviceconfig;

// handlers["comment"].function(isCommentLine(line), line);
// handlers["define"].function(isDefineLine(line), line);
// handlers["empty"].function(isEmptyOrComment(line),line);

bool isCommentLine(const std::string &line)
{
    return line.find("/**") != std::string::npos;
}

bool isDefineLine(const std::string &line)
{
    return line.find("#define") != std::string::npos;
}

bool isEmptyOrComment(const std::string &line)
{
    return line.empty() || isCommentLine(line);
}

DeviceConfig &DeviceConfig::getInstance(const string &filename)
{
}

void DeviceConfig::initHandlers()
{
    line_processor["comment"] = {[this](bool condition, const std::string &line)
                                 {
                                     if (condition)
                                         processCommentLine(line);
                                 }};

    line_processor["define"] = {[this](bool condition, const std::string &line)
                                {
                                    if (condition && context_.in_define_section)
                                        processDefineLine(line);
                                }};

    line_processor["empty"] = {[this](bool condition, const std::string &line)
                               {
                                   if (condition)
                                       context_.reset();
                               }};
}

void DeviceConfig::processCommentLine(const std::string &line)
{
    if (line.find("/**I") != std::string::npos || line.find("/**O") != std::string::npos)
    {
        size_t start = line.find("VL_") + 3;
        size_t end = line.find(" */", start);
        std::string vl_str = line.substr(start, end - start);
        try
        {
            context_.current_vl_id = std::stoi(vl_str);
            context_.is_input = (line.find("/**I") != std::string::npos);
            context_.in_define_section = true;
        }
        catch (const std::invalid_argument &e)
        {
            throw std::runtime_error("无效的 VL_ID 值: " + vl_str);
        }
    }
}

void DeviceConfig::storeAttribute(const std::string &key, const std::string &value)
{
    // PortInfo &current_port_info = context_.is_input ? context_.input_map[context_.current_port_value]
    //                                                 : context_.output_map[context_.current_port_value];
    // current_port_info.attributes[key] = value;
}

void DeviceConfig::processDefineLine(const std::string &line)
{
    std::istringstream iss(line);
    std::string define, key, value;
    iss >> define >> key >> value;

    value = value.substr(1, value.size() - 2);

    size_t first_underscore = key.find('_');
    size_t second_underscore = key.find('_', first_underscore + 1);
    if (second_underscore != std::string::npos)
    {
        key = key.substr(second_underscore + 1);
        key.erase(std::remove_if(key.begin(), key.end(), ::isdigit), key.end());
    }
    else
    {
        return;
    }

    if (key == "PORT_ID")
    {
        handlePortId(value);
    }
    else if (context_.current_port_value != -1)
    {
        storeAttribute(key, value);
    }
}

void DeviceConfig::handlePortId(const std::string &value)
{
    try
    {
        context_.current_port_value = std::stoi(value);
        PortInfo port_info(context_.current_vl_id);
        if (context_.is_input)
        {
            // DeviceMap[context_.current_port_value] = port_info;
            // context_.input_map[context_.current_port_value] = port_info;
        }
        else
        {
            // context_.output_map[context_.current_port_value] = port_info;
        }
    }
    catch (const std::invalid_argument &e)
    {
        std::cerr << "无效的 PORT_ID 值: " << value << std::endl;
    }
}

void DeviceConfig::handleLine(const std::string &line)
{
    // 如果是注释行，就处理注释
    line_processor["comment"].function(isCommentLine(line), line);
    // 如果是宏定义行，就处理宏定义
    line_processor["define"].function(isDefineLine(line), line);
    // 如果是空行
    line_processor["empty"].function(isEmptyOrComment(line), line);
}

int DeviceConfig::HFileParser(const std::string &filename)
{
    std::ifstream file(filename);
    context_.reset();

    if (!file.is_open())
    {
        std::cerr << "无法打开文件: " << filename << std::endl;
        return -1;
    }

    try
    {
        std::string line;
        while (std::getline(file, line))
        {
            handleLine(line); // 可能抛出异常
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "解析失败: " << e.what() << std::endl;
        return -2; // 返回错误码表示解析失败
    }

    return 0; // 成功解析
}

// 解析端口信息，这里面应该有很多的.h文件，
int DeviceConfig::parsePortInfoFile(const std::string &dirPath)
{
    vector<fs::path> portInfoFilePaths = this->getPortInfoFilePath(dirPath, PORTFILEPREFIX);
    for (fs::path filename : portInfoFilePaths)
    {
        int result = HFileParser(filename.string());
        if (result != 0)
            return -1;
    }
    return 0;

    // std::for_each(portInfoFilePaths.begin(), portInfoFilePaths.end(), HFileParser);

}

// 检查文件是否有效
bool DeviceConfig::isValidHeaderFile(const fs::path &file, const std::string &searchString)
{
    return fs::is_regular_file(file) &&
           file.extension() == ".h" &&
           file.filename().string().find(searchString) != std::string::npos;
}

vector<fs::path> DeviceConfig::getPortInfoFilePath(const string &dirPath, const string &searchString)
{
    vector<fs::path> portInfoFilePaths;

    // 检查目录是否存在
    if (!fs::exists(dirPath))
    {
        std::cerr << "Directory does not exist: " << dirPath << std::endl;
        return portInfoFilePaths; // 返回空向量
    }

    if (!fs::is_directory(dirPath))
    {
        std::cerr << "Path is not a directory: " << dirPath << std::endl;
        return portInfoFilePaths; // 返回空向量
    }

    // 遍历目录
    for (const auto &entry : fs::recursive_directory_iterator(dirPath))
    {
        if (this->isValidHeaderFile(entry.path(), searchString))
        {
            portInfoFilePaths.push_back(entry.path());
        }
    }

    return portInfoFilePaths;

    // return full_path.string();
}
