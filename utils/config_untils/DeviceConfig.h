#include <iostream>
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <xlnt/xlnt.hpp>

#include <mutex>

using namespace std;
using namespace xlnt;
// excle
#define SERVICE_PLAN_SHEET 0
#define NETWORK_CONFIG_SHEET 1

#define MAC_COL 13
#define DEVICE_NUM_COL 4
#define BUS_TYPE_COL 14
#define VL_ID_COL 17
#define IP_SRC_COL 5
#define IP_DST_COL 12
#define UDP_PORT_COL 3

// 设备端口信息类,存储这每一个设备的端口信息，也就是所有设备的端口都放在这里
struct PortInfo
{
    int vl_id; // 端口的 VL_ID
    unordered_map<string, string> attributes;

    // 默认构造函数
    PortInfo()
        : vl_id(-1), attributes() {}

    // 带参数的构造函数
    PortInfo(int vl_id)
        : vl_id(vl_id), attributes() {}
};

struct ServiceData
{

    char vl_id[2] = {0};
    char ip_src[4] = {0};
    char ip_dst[4] = {0};
    char bus_type[2] = {0};
    char udp_port_src[2] = {0};
    char udp_port_dst[2] = {0};
};

struct DeviceData
{
    char mac[6] = {0};
};

// 设备数据：描述一个设备的基本信息
struct DeviceData
{
    char mac[6] = {0};                             // 设备 MAC 地址
    unordered_map<int, PortInfo> in_ports;       // 设备的端口信息   vl_id - 端口
    unordered_map<int, PortInfo> out_ports;       // 设备的端口信息   vl_id - 端口

    unordered_map<int, ServiceData> serviceData; // 设备提供的服务  vl_id - 服务数据

    DeviceData() = default;
};

// 设备的映射：设备的id -> 设备信息
using DeviceMap = std::unordered_map<int, DeviceData>;

// 设备配置类
// 这个是要管理所有的设备信息
// 1.一开始是要将所有的设备信息全部加载
// 2.留获取一个设备信息的口和多个设备信息的口
// 3.用到这个时，需要能够根据设备的tte_id进行搜查
// 4.其实这个感觉也可以是一个单例模式，全局只有一个对象掌管设备信息，然后用的时候去查找，这个时候确实是符合这个unique
class DeviceConfig
{
public:
    static DeviceConfig &getInstance(const string &filename); // 获取单例实例
    void load(const string &filename);    // 从文件夹加载配置
    DeviceData getDeviceDataByID(int deviceId);

private:
    xlnt::workbook wb;
    // 解析端口数据-解析.h文件
    int parsePortInfo(const std::string &filename);
    // 解析设备数据和服务数据-解析excel
    int parseExcel(workbook wb);
    bool isValidHeaderFile(const fs::path &file, const std::string &searchString);
    vector<fs::path> getPortInfoFilePath(const string &dirPath, const string &searchString);
};
