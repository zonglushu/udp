#ifndef PARSE_H_
#define PARSE_H_
#include <stdint.h>
#include <vector>
#include "udp_utils/udp_access.h"
#include "udp_utils/UDPRcvRawMessage.h"

#include <bitset>
#include <cstring>
#include <iostream>

#include <fstream>
// #include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>


// #include "TTEAccess.h"
#include "parseFile.h"
#define VERSION_MASK 0xE000
#define TYPE_MASK 0x1000
#define DEPUTY_GUIDE_HEAD_ID_MASK 0x800
#define APPLY_PROCESS_ID_MASK 0x7ff
#define GROUP_ID_MASK 0xc000
#define PACKEGE_COUNT_MASK 0x3fff

#define VERSION_OFFSET 13
#define TYPE_OFFSET 12
#define DEPUTY_GUIDE_HEAD_ID_OFFSET 11
#define APPLY_PROCESS_ID_OFFSET 0
#define GROUP_ID__OFFSET 14
#define PACKEGE_COUNT_OFFSET 0


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


// 协议类型
enum class ProtocolType {
    Kprotocol1,
    Kprotocol2,
    Kprotocol3,
    Kprotocol4,
    Kprotocol5,
    Kprotocol6,
    Kother
};


class DataField {
using PortInfoMap = std::unordered_map<int, PortInfo>;

    friend int tteFrame_to_buffer(DataField& data_field,int vl_id,ServiceData service_data,DeviceData device_data ,char* buffer);
   public:
    DataField(std::vector<uint8_t> data);
    DataField();
    ~DataField();

    // 用户数据

     uint16_t  apply_process_id_ = 0;
     int getApplyProcessId() const { return apply_process_id_; }
     int modify(const char *buffer);
     
     uint16_t getAppId();

     bool is_success_ =false;

   private:
    uint16_t version_ = 0;
    uint16_t type_ = 0;
    uint16_t deputy_guide_head_id_ = 0;
   
    uint16_t group_id_ = 0;
    uint16_t packet_count_ = 0;
    uint16_t packet_len_ = 0;
    uint8_t packet_deputy_guide_head_[10] = {0};
    std::vector<uint8_t> data_;
    uint16_t verify_field_;
    std::vector<uint8_t> toBuffer();

};


std::string intToHex(int value);

std::string toHexString(const std::vector<uint8_t>& vec) ;
std::string toHexString(const char* data, size_t length);
size_t hexStringToCharArray(const std::string& hexStr, char* charArray);
#endif