
#include <iostream>
#include <fstream>
#include <sstream>

#include <utility>  // for std::pair
#include <algorithm>
#include <stdexcept>
#include "parseFile.h"

#include "timer.h"
#include <map>
#include "easylogging++.h"


int parsePortInfo(const std::string& filename, PortInfoMap& input_map, PortInfoMap& output_map) {
    std::ifstream file(filename);
    std::string line;
    int current_port_value = -1;
    int current_vl_id = -1;
    bool is_input = false;
    bool in_define_section = false; // 新增标记变量

    if (!file.is_open()) {
        std::cerr << "无法打开文件: " << filename << std::endl;
        return -1;
    }

    while (std::getline(file, line)) {
        // 处理注释行以获取 VL_ID
        if (line.find("/**I") != std::string::npos) {
            size_t start = line.find("VL_") + 3;
            size_t end = line.find(" */", start);
            std::string vl_str = line.substr(start, end - start);
            try {
                current_vl_id = std::stoi(vl_str);
                is_input = true;
                in_define_section = true; // 进入定义部分
            } catch (const std::invalid_argument& e) {
                std::cerr << "无效的 VL_ID 值: " << vl_str << std::endl;
                return -1;
            }
        } else if (line.find("/**O") != std::string::npos) {
            size_t start = line.find("VL_") + 3;
            size_t end = line.find(" */", start);
            std::string vl_str = line.substr(start, end - start);
            try {
                current_vl_id = std::stoi(vl_str);
                is_input = false;
                in_define_section = true; // 进入定义部分
            } catch (const std::invalid_argument& e) {
                std::cerr << "无效的 VL_ID 值: " << vl_str << std::endl;
                return -1;
            }
        }
        // 处理宏定义行
        else if (line.find("#define") != std::string::npos) {
            if (!in_define_section) {
                continue; // 如果不在定义部分，则跳过此行
            }

            std::istringstream iss(line);
            std::string define, key, value;
            iss >> define >> key >> value;
            
            value = value.substr(1, value.size() - 2);  // 去掉括号或引号

            // 提取第二个下划线后的部分作为键名
            size_t first_underscore = key.find('_');
            size_t second_underscore = key.find('_', first_underscore + 1);
            if (second_underscore != std::string::npos) {
                key = key.substr(second_underscore + 1);
                key.erase(std::remove_if(key.begin(), key.end(), ::isdigit), key.end());
            } else {
                continue;  // 如果找不到第二个下划线，则跳过此行
            }

            // 处理 PORT_ID 行
            if (key == "PORT_ID") {
                try {
                    current_port_value = std::stoi(value);
                    PortInfo port_info(current_vl_id);
                    if (is_input) {
                        input_map[current_port_value] = port_info;
                    } else {
                        output_map[current_port_value] = port_info;
                    }
                } catch (const std::invalid_argument& e) {
                    std::cerr << "无效的 PORT_ID 值: " << value << std::endl;
                    return -1;
                }
            }
            // 存储其他属性
            if (current_port_value != -1) {
                if (is_input) {
                    PortInfo& current_port_info = input_map[current_port_value];
                    current_port_info.attributes[key] = value;
                } else {
                    PortInfo& current_port_info = output_map[current_port_value];
                    current_port_info.attributes[key] = value;
                }
            }
        } else if (line.empty() || line.find("/**") != std::string::npos) {
            // 遇到空行或者新的注释行，重置标记
            in_define_section = false;
        }
    }

    return 0;
}

// 查找vlid信息
int get_vl_Id(const PortInfoMap& port_map, int port) {

    auto it = port_map.find(port);
    if (it != port_map.end()) {
   
        return it->second.vl_id;
    }
    return -1;
}
void print_all_ports(const PortInfoMap& port_map) ;
int get_attribute(const PortInfoMap& port_map, int port, const std::string& key, std::string& result) {
    // print_all_ports(port_map);
    // return -1;
    auto it = port_map.find(port);
    if (it != port_map.end()) {
        const PortInfo& port_info = it->second;
        auto attr_it = port_info.attributes.find(key);
        if (attr_it != port_info.attributes.end()) {
            result = attr_it->second;
            return 0;  // 成功找到字段
        }
    }
    // result = "";  // 设置结果为空，表示未找到
    return -1;  // 字段或端口未找到
}

/* debug */
void print_all_ports(const PortInfoMap& port_map) {
    for (const auto& pair : port_map) {
        int port = pair.first;
        const PortInfo& port_info = pair.second;
        
        std::cout << "Port: " << port << std::endl;
        std::cout << "  VL_ID: " << port_info.vl_id << std::endl;
        std::cout << "  Attributes:" << std::endl;

        for (const auto& attr : port_info.attributes) {
            std::cout << "    " << attr.first << ": " << attr.second << std::endl;
        }
    }
}




int find_row_with_value(xlnt::worksheet ws, int column, const std::string& target_value) {
   
  try {
    // 选择指定的工作表
    // xlnt::worksheet ws = wb.sheet_by_title(sheet_name);
      
    auto rows = ws.rows();
        
    std::size_t row_count = std::distance(rows.begin(), rows.end());
      

     // 遍历指定列的所有单元格
    for (int i=1; i <= row_count; i++)
    {
       auto currentStr= ws.cell(column,i).to_string();

       if(currentStr==target_value){
        return i;
       }
    }
    } catch (const std::exception& e) {
        std::cerr << "Failed to find sheet: " << e.what() << std::endl;
        return -1;
    }

    // 如果没有找到目标值，返回 -1
    return -1;
}


int loadExcel(const std::string& filename,xlnt::workbook& wb_load){
  
    try{
        wb_load.load(filename);
        return 0;
    }
    catch(const std::exception& e){
        std::cerr << e.what() << '\n';
        return -1;
    }
}

// mac

#define SERVICE_PLAN_SHEET 0
#define NETWORK_CONFIG_SHEET 1

#define MAC_COL 13
#define DEVICE_NUM_COL 4
#define BUS_TYPE_COL 14
#define VL_ID_COL 17
#define IP_SRC_COL 5
#define IP_DST_COL 12
#define UDP_PORT_COL 3


std::string trim(const std::string& str) {
    std::string trimmed_str = str;
    trimmed_str.erase(0, trimmed_str.find_first_not_of(" \t\n\r\f\v"));
    trimmed_str.erase(trimmed_str.find_last_not_of(" \t\n\r\f\v") + 1);
    return trimmed_str;
}

int ipStringToBuffer(const std::string& ip, char buffer[4]) {
    std::istringstream ipStream(ip);
    std::string part;

    for (int i = 0; i < 4; ++i) {
        // 从输入流中读取一部分内容到 part 中，以 '.' 作为分隔符
        if (!std::getline(ipStream, part, '.')) {
            return -1;
        }
        int byte = 0;
        // 使用 C++11 之前的方式转换字符串为整数
        char* endPtr;
        byte = static_cast<int>(strtol(part.c_str(), &endPtr, 10));
        // 如果转换后还有未处理的字符，说明转换失败
        if (*endPtr!= '\0') {
            return -1;
        }
            // 判断转换后的整数是否在合法范围内（0-255）
        if (byte < 0 || byte > 255) {
            return -2;
        }
        buffer[i] = static_cast<char>(byte);
        }

        if (std::getline(ipStream, part, '.')) {
            return -1;
        }

        return 0;
}
int parseMacAddress(const std::string& mac, char result[6]) {
    std::stringstream ss(mac);
    std::string segment;
    int index = 0;
    // 只处理6字节
    try {
        while (std::getline(ss, segment, '-') && index < 6) {
            uint8_t byte = static_cast<uint8_t>(std::stoul(segment, nullptr, 16));
            result[index++] = static_cast<char>(byte);
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return -1;
    }

    if (index!= 6) {
        return -1;
    }

    return 0;
}



int hexStringToInt(const std::string& hexStr, int& outValue) {
    // 检查输入是否为空字符串
    if (hexStr.empty()) {
        return -1; // 返回1表示失败
    }

    // 使用strtoul将字符串转换为无符号长整型
    errno = 0; // 重置errno
    char* endptr;
    unsigned long value = strtoul(hexStr.c_str(), &endptr, 16);

    // 检查转换是否成功
    if (errno != 0 || *endptr != '\0') {
        return -1; // 返回1表示失败
    }

    // 检查值是否超出uint16_t的范围
    if (value > 0xFFFF) {
        return -1; // 返回1表示失败
    }

    outValue = static_cast<int>(value);
    return 0; // 返回0表示成功
}


int numberToBytes(int value, size_t byteCount, std::vector<uint8_t>& result, bool bigEndian) {
    // 确保 byteCount 在合理范围内
    if (byteCount > sizeof(int)) {
        return -1; // 返回 -1 表示 byteCount 超过了类型的实际字节数
    }

    // 确保 result 有足够的空间
    result.resize(byteCount, 0); // 初始化为 0，处理填充情况

    // 根据字节序填充字节数组
    for (size_t i = 0; i < byteCount; ++i) {
        if (bigEndian) {
            result[byteCount - 1 - i] = static_cast<uint8_t>(value >> (i * 8));
        } else {
            result[i] = static_cast<uint8_t>(value >> (i * 8));
        }
    }

    return 0; // 成功
}


std::map<std::string, std::vector<uint8_t>> bus_type_map = {
    {"TT", {0x88,0xd7}},
    {"RC", {0x08,0x88}},
    {"BE", {0x08,0}},
    {"PCF", {0x89,0x1d}},
};

int parseExcel(xlnt::workbook wb,ServiceMap& service_map,DeviceMap& device_map){
     try {
        xlnt::worksheet service_plan_sheet = wb.sheet_by_index(SERVICE_PLAN_SHEET);
        xlnt::worksheet netWork_config_sheet = wb.sheet_by_index(NETWORK_CONFIG_SHEET);
        
        auto service_plan_sheet_rows = service_plan_sheet.rows();
        auto netWork_config_sheet_rows = netWork_config_sheet.rows();

        std::size_t service_row_count = std::distance(service_plan_sheet_rows.begin(), service_plan_sheet_rows.end());
        std::size_t netWork_row_count = std::distance(netWork_config_sheet_rows.begin(), netWork_config_sheet_rows.end());

        for (int i=2; i <= service_row_count; i++){
            ServiceData service_data;
            auto vl_id_str= trim(service_plan_sheet.cell(VL_ID_COL,i).to_string());
            auto ip_src_str= trim(service_plan_sheet.cell(IP_SRC_COL,i).to_string());
            auto ip_dst_str= trim(service_plan_sheet.cell(IP_DST_COL,i).to_string());
            auto bus_type_str= trim(service_plan_sheet.cell(BUS_TYPE_COL,i).to_string());
            auto udp_port_str= trim(service_plan_sheet.cell(UDP_PORT_COL,i).to_string());

            int vl_id;
            int vl_id_res=hexStringToInt(vl_id_str,vl_id);
             if(vl_id_str=="/"){
                continue;
            }
            if(vl_id_res<0){
                CLOG(ERROR,TTE_LOGGER)<<"解析vl_id__str失败 "<<vl_id_str;
                continue;
                // return -1;
            }
            
            // 如果已经有该vl_id信息，跳过
            auto vl = service_map.find(vl_id);
            if(vl != service_map.end()){
                continue;
            }
            std::vector<uint8_t> vl_id_vec;
            int vl_vec_res=numberToBytes(vl_id,2,vl_id_vec);
            if(vl_vec_res<0){
                CLOG(ERROR,TTE_LOGGER)<<"numberToBytes vl_id失败 "<<vl_id;

                return -1;
            }
            
            std::copy(vl_id_vec.begin(),vl_id_vec.end(), service_data.vl_id);

            int ip_src_res= ipStringToBuffer(ip_src_str,service_data.ip_src);
            if(ip_src_res<0){
                CLOG(ERROR,TTE_LOGGER)<<"解析ip_src_str失败 "<<ip_src_str<<"长度="<<ip_src_str.size()<<"res"<<ip_src_res;
                return -1;
            }
            int ip_dst_res= ipStringToBuffer(ip_dst_str,service_data.ip_dst);
            if(ip_dst_res<0){
                CLOG(ERROR,TTE_LOGGER)<<"解析ip_dst_str失败 "<<ip_dst_str<<"长度="<<ip_dst_str.size() <<"res"<<ip_dst_res;

                return -1;
            }
             auto it = bus_type_map.find(bus_type_str);
            if(it == bus_type_map.end()){
                CLOG(ERROR,TTE_LOGGER)<<"解析bus_type_str失败 "<<bus_type_str;
                return -1;
            }
            auto bus_type=it->second;            
            std::copy(bus_type.begin(),bus_type.end(), service_data.bus_type);
            
            int udp_port=stoi(udp_port_str);
            if(udp_port<0){
                CLOG(ERROR,TTE_LOGGER)<<"解析udp_port_str失败 "<<udp_port_str;

                return -1;
            }
            std::vector<uint8_t> udp_port_vec;
            int udp_port_vec_res=numberToBytes(udp_port,2,udp_port_vec);
            if(udp_port_vec_res<0){
                CLOG(ERROR,TTE_LOGGER)<<"numberToBytes udp_port失败 "<<udp_port;

                return -1;
            }
            std::copy(udp_port_vec.begin(),udp_port_vec.end(), service_data.udp_port_dst);
            std::copy(udp_port_vec.begin(),udp_port_vec.end(), service_data.udp_port_src);


            service_map.insert(std::pair<int, ServiceData>(vl_id, service_data));
        }

        // 遍历指定列的所有单元格
        for (int i=2; i <= netWork_row_count; i++){
            auto device_name_str= netWork_config_sheet.cell(DEVICE_NUM_COL,i).to_string();
            DeviceData device_data;
            auto mac_str= netWork_config_sheet.cell(MAC_COL,i).to_string();

            int mac_res= parseMacAddress(mac_str,device_data.mac);
            if(mac_res<0){
                CLOG(ERROR,TTE_LOGGER)<<"解析 mac_str失败 "<<mac_str;

                return -1;
            }
        
            device_map.insert(std::pair<std::string, DeviceData>(device_name_str,device_data));
        }
     } catch (const std::exception& e) {
            std::cerr << "Failed to find sheet: " << e.what() << std::endl;
            return -1;
    }

    return 0;

}


// debug用
void printDeviceMap(const DeviceMap& deviceMap) {
        printf("%dSize",deviceMap.size());
    for (const auto& pair : deviceMap) {
        printf("Key: %s\n", pair.first.c_str());
        const DeviceData& data = pair.second;

        printf("mac: ");
        for (int i = 0; i < 6; ++i) {
            printf("%02x ", static_cast<uint8_t>(data.mac[i]));
        }
        printf("\n");
    }
}
void printServiceMap(const ServiceMap& serviceMap) {
    for (const auto& pair : serviceMap) {
        printf("Key: %d\n", pair.first);
        const ServiceData& data = pair.second;

        printf("ip_src: ");
        for (int i = 0; i < 4; ++i) {
            printf("%02x ", static_cast<uint8_t>(data.ip_src[i]));
        }
        printf("\n");

        printf("ip_dst: ");
        for (int i = 0; i < 4; ++i) {
            printf("%02x ", static_cast<uint8_t>(data.ip_dst[i]));
        }
        printf("\n");

        printf("bus_type: ");
        for (int i = 0; i < 2; ++i) {
            printf("%02x ", static_cast<uint8_t>(data.bus_type[i]));
        }
        printf("\n");

        printf("udp_port_src: ");
        for (int i = 0; i < 2; ++i) {
            printf("%02x ", static_cast<uint8_t>(data.udp_port_src[i]));
        }
        printf("\n");

        printf("udp_port_dst: ");
        for (int i = 0; i < 2; ++i) {
            printf("%02x ", static_cast<uint8_t>(data.udp_port_dst[i]));
        }
        printf("\n");
    }
}