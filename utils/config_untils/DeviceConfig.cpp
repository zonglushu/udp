#include "DeviceConfig.h"
#include <boost/filesystem.hpp>

#define PORTFILEPREFIX "stream"
static once_flag initFlag;    // 初始化标记
static string configFilename; // 配置文件名

using namespace boost::filesystem;
namespace fs = boost::filesystem;
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



void DeviceConfig::handleLine(const std::string &line)
{
    line_processor["comment"].function(isCommentLine(line), line);
    line_processor["define"].function(isDefineLine(line), line);
    line_processor["empty"].function(isEmptyOrComment(line), line);
}

// 解析端口信息，这里面应该有很多的.h文件，
int DeviceConfig::parsePortInfo(const std::string &dirPath)
{
    vector<fs::path> portInfoFilePaths = this->getPortInfoFilePath(dirPath, PORTFILEPREFIX);
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
