#include <string>
#include <unordered_map>

#include <xlnt/xlnt.hpp>
#include "timer.h"

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

struct PortInfo
{
    int vl_id; // 端口的 VL_ID
    std::unordered_map<std::string, std::string> attributes;

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

// 定义 PortInfoMap 类型
using PortInfoMap = std::unordered_map<int, PortInfo>;

int loadExcel(const std::string &filename, xlnt::workbook &wb);
