#ifndef PARSE_FILE_H_
#define PARSE_FILE_H_

#include <string>
#include <unordered_map>

#include <xlnt/xlnt.hpp>
#include "timer.h"

// 定义 PortInfo 结构体
// PortInfo 结构体定义
struct PortInfo {
    int vl_id;  // 端口的 VL_ID
    std::unordered_map<std::string, std::string> attributes;

    // 默认构造函数
    PortInfo()
        : vl_id(-1), attributes() {}

    // 带参数的构造函数
    PortInfo(int vl_id)
        : vl_id(vl_id), attributes() {}
};
struct ServiceData{

    char vl_id[2]={0};
    char ip_src[4]={0};
    char ip_dst[4]={0};
    char bus_type[2]={0};
    char udp_port_src[2]={0};
    char udp_port_dst[2]={0};
   
};

struct DeviceData
{
    char mac[6]={0};
};


int numberToBytes(int value, size_t byteCount, std::vector<uint8_t>& result, bool bigEndian = true);

using ServiceMap = std::unordered_map<int, ServiceData>;

using DeviceMap = std::unordered_map<std::string,DeviceData>;

int parseExcel(xlnt::workbook wb,ServiceMap& service_map,DeviceMap& device_map);

void printServiceMap(const ServiceMap& serviceMap);
void printDeviceMap(const DeviceMap& deviceMap);


// 定义 PortInfoMap 类型
using PortInfoMap = std::unordered_map<int, PortInfo>;

int parsePortInfo(const std::string& filename, PortInfoMap& input_map, PortInfoMap& output_map);
int get_vl_Id(const PortInfoMap& port_map, int port) ;
int get_attribute(const PortInfoMap& port_map, int port, const std::string& key, std::string& result);



int loadExcel(const std::string& filename,xlnt::workbook& wb);



int find_row_with_value(xlnt::worksheet ws , int column, const std::string& target_value) ;

#endif