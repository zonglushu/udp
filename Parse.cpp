#include "Parse.h"
#include "timer.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include <sstream>
#include <cstdint>

#include <algorithm>
// #include "time.h"
#include "timer.h"
#include "easylogging++.h"

#include "tte_linux_lib.h"


int ISO_SOlve(const std::vector<uint8_t> &data, size_t length, char *result)
{
    if (length < 2)
    {
        return -1; // 如果长度小于2，返回失败
    }

    int c0 = 0;
    int c1 = 0;

    for (size_t i = 0; i < length; ++i)
    {
        c0 += data[i]; // 累加到 c0
        c1 += c0;      // 累加到 c1
    }

    int ck1 = ((-(c0 + c1)) % 255) - 1; // 计算 ck1
    int ck2 = c1 % 255;                 // 计算 ck2
    ck1 = ck1 + 1;

    if (ck2 == 0)
    {
        ck2 = 255; // 如果 ck2 为 0，则将其设为 255
    }

    // 将 ck1 和 ck2 转换为实际的校验值
    result[0] = static_cast<char>((255 + ck1) & 0xFF);
    result[1] = static_cast<char>(ck2 & 0xFF);

    return 0; // 成功
}

int computeCheckSum(const char *input, size_t length, char &result_high, char &result_low)
{
    std::vector<uint8_t> input_v(input, input + length);
    if (input_v.size() % 2 != 0)
    {
        // 输入大小不为偶数，补齐
        input_v.push_back(0);
    }

    unsigned long hexVal = 0;
    unsigned long hexVal1 = 0;

    size_t numElements = input_v.size();

    // 计算校验和
    for (size_t i = 0; i < numElements / 2; ++i)
    {
        hexVal += (input_v[i * 2] * 256) + input_v[i * 2 + 1];
    }

    // 将校验和折叠到 16 位
    do
    {
        hexVal1 = hexVal % 65536;
        hexVal = (hexVal - hexVal1) / 65536;
        hexVal += hexVal1;
    } while (hexVal > 65536);

    // 计算最终校验和
    uint16_t checksum = static_cast<uint16_t>(65535 - hexVal);

    // 将校验和存储在结果引用中
    result_high = static_cast<char>((checksum >> 8) & 0xFF); // 高字节
    result_low = static_cast<char>(checksum & 0xFF);         // 低字节

    return 0; // 成功
}
// 将整数转换为十六进制字符串
std::string intToHex(int value)
{

    char buffer[7];   // 4位十六进制 + 2位 "0x" + 1位 null 终止符
    buffer[6] = '\0'; // null 终止符

    // 在字符串开头添加 "0x"
    buffer[0] = '0';
    buffer[1] = 'x';

    // 格式化整数为四位十六进制，并填充前导零
    std::sprintf(buffer + 2, "%04X", value);

    return std::string(buffer);
}

uint16_t DataField::getAppId(){
    return apply_process_id_;
}


// 整个数据域的定义过程
DataField::DataField(std::vector<uint8_t> data)
    : data_(data)
{
    auto real_length=data.size();

    if(real_length<18){
        CLOG(ERROR,TTE_LOGGER)<<"数据域长度小于最小长度（18）"<<"长度"<<data.size();
    }

    uint16_t byte0_1 = (data_[0] << 8) + data_[1];
    // 版本号
    version_ = (byte0_1 & VERSION_MASK) >> VERSION_OFFSET;
    // 类型
    type_ = (byte0_1 & TYPE_MASK) >> TYPE_OFFSET;
    // 副导报文头标识
    deputy_guide_head_id_ = (byte0_1 & DEPUTY_GUIDE_HEAD_ID_MASK) >> DEPUTY_GUIDE_HEAD_ID_OFFSET;
    // 应用进程标识符
    apply_process_id_ = (byte0_1 & APPLY_PROCESS_ID_MASK) >> APPLY_PROCESS_ID_OFFSET;

    
    uint16_t byte2_3 = (data_[2] << 8) + data_[3];
    // 分组标识
    group_id_ = (byte2_3 & GROUP_ID_MASK) >> GROUP_ID__OFFSET;
    // 包序列计数
    packet_count_ = (byte2_3 & PACKEGE_COUNT_MASK) >> PACKEGE_COUNT_OFFSET;
    // 包数据长度
    packet_len_ = (data_[4] << 8) + data_[5];

    if(packet_len_+7>real_length){
        CLOG(ERROR,TTE_LOGGER)<<"包实际长度超过实际长度 "<<"包长度 = "<<packet_len_<<"\n"<<"real_length = "<<real_length;
        CLOG(ERROR,TTE_LOGGER)<<toHexString(((char*)data_.data()),2);
        
        return;
    }

    for (int i = 0; i < 10; i++)
    {
        packet_deputy_guide_head_[i] = data_[i + 6];
    }

    // 校验域
    verify_field_ = (data_[packet_len_+7- 2] << 8) + data_[packet_len_+7 - 1];

    CLOG(DEBUG,TTE_LOGGER)<<"byt5";

    is_success_=true;
};

DataField::DataField() {};
DataField::~DataField()
{
    // delete userdata;
}

std::vector<uint8_t> DataField::toBuffer()
{
 
    // 校验域
    char resultData[2] = {0, 0};

    int res = ISO_SOlve(data_, packet_len_ + 7 - 2, resultData);
    CLOG_IF(res < 0, ERROR, TTE_LOGGER) << "ISO校验失败";

    data_[packet_len_ + 7 - 2] = resultData[0];
    data_[packet_len_ + 7 - 1] = resultData[1];
    CLOG(DEBUG,TTE_LOGGER)<<"原校验位 "<< intToHex(verify_field_);
    CLOG(DEBUG,TTE_LOGGER)<<"重新校验用户数据 "<< toHexString(resultData,2);

    return data_;
}
// 判断是否为全FF
bool isAllFF(const char *byteArray, size_t length)
{
    for (size_t i = 0; i < length; ++i)
    {
        if (static_cast<uint8_t>(byteArray[i]) != 0xFF)
        {
            return false;
        }
    }
    return true;
}
// 将字节数组转换为无符号64位整数
uint64_t bytesToNumber(const char *byteArray, size_t length)
{
    uint64_t number = 0;
    for (size_t i = 0; i < length; ++i)
    {
        number = (number << 8) | static_cast<uint8_t>(byteArray[i]);
    }
    return number;
}

// #define P_SYNC_HEAD_LEN 0
// #define P_LENGTH_LEN 4
// #define P_OPERATE_TYPE_LEN 4
// #define P_VERIFY 2

// #define READ_BUFFER_SIZE 1024 * 1024 * 2

// 解析的是IPv4头里面的数据


// 返回-1,解析失败，其它情况返回1，打开日志，0 ，关闭日志
int DataField::modify(const char *buffer){
    
    int count=0;
    // 取出总长度（4字节）大端序
    uint32_t totalLength = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3] + 4;
    CLOG(DEBUG,TTE_LOGGER)<<"协议长度"<<totalLength;
    if(totalLength>READ_BUFFER_SIZE){
        CLOG(ERROR,TTE_LOGGER)<<"readbuffer越界";
        return -1;
    }

    // 判断是否有效协议（0x00000A0A 有效 全FF无效 ，其他无效）
    bool isValid = false;
    for (size_t i = 4; i < 8; ++i) {
        if (uint8_t(buffer[i]) != 0xFF) {
            isValid = true;
            break;
        }
    }
    
    size_t offset = 8;
    if (isValid) {
        // 开始循环解析数据单元,当没有超过有效数据长度时，考虑填充字节aa,如果数据单元长度解析到aa,判断解析该长度是否超过有效数据长度
       
        int a=0;
        while (offset < totalLength - 1) {
            // printf("offset:%d\n",offset);
            // printf("totalLength:%d\n",totalLength );

            // 取出数据单元长度（1字节）
            uint8_t unitLength = buffer[offset];
            // 如果数据单元长度解析到aa,判断解析该长度是否超过有效数据长度
            if (unitLength == 0xAA && totalLength - offset - 1 < 0xAA) {
                offset += 1;
                // std::cout << "有补位" << std::endl;
                break;
            }

            // 取出标识符（2字节）大端序
            uint16_t identifier = (buffer[offset + 1] << 8) | buffer[offset + 2];

            // 取出起始索引（2字节）大端序
            uint16_t startIndex = (buffer[offset + 3] << 8) | buffer[offset + 4];
            // 取出结束索引（2字节）大端序
            uint16_t endIndex = (buffer[offset + 5] << 8) | buffer[offset + 6];

            // 计算要修改的字节数
            size_t modifyLength = endIndex - startIndex + 1;

            
            // 判断该包是否被用于修改 identifier==apply_process_id_
            if(identifier==apply_process_id_){
                count++;

                for (size_t i = startIndex; i<=endIndex; i++){
                    
                    data_[i+16] = buffer[offset + 7 + i - startIndex]&buffer[offset + 7 + i - startIndex + modifyLength];

                }
                
            }

        offset=offset+unitLength+1;
        
    }
     if(offset!=totalLength-1){
         return -1;
    }     
}else{
     CLOG(DEBUG,TTE_LOGGER)<<"无需修改";
     printf("%d %d",uint8_t((buffer+totalLength-1)[0]),totalLength-1);
     return uint8_t((buffer+totalLength-1)[0])==1;
} 
    CLOG(DEBUG,TTE_LOGGER)<<"修改"<<count<<"处";
    return uint8_t((buffer+totalLength-1)[0])==1;
}


// 组包代码：进行组包的数据为目的mac（未在代码中出现，可能为自动填充？）、目的mac、ipv4头、udp头、数据内容
int tteFrame_to_buffer(DataField &data_field, int vl_id, ServiceData service_data, DeviceData device_data, char *buffer)
{


    std::vector<uint8_t> data = data_field.toBuffer();

    int udp_data_len = data.size() + 8;
    int ip_v4_data_len = 20 + udp_data_len;

    // 目的MAC
    char mac_dst[6] = {
        0xAB, 0XAD, 0xBA, 0xBE, service_data.vl_id[0], service_data.vl_id[1]};
    int index = 0;
    std::memcpy(buffer, mac_dst, 6);
    index += 6;
    std::memcpy(buffer + index, device_data.mac, 6);
    index += 6;
    std::memcpy(buffer + index, service_data.bus_type, 2);
    index += 2;

    // ipv4
    std::vector<uint8_t> ip_v4_len;
    int ip_v4_len_res = numberToBytes(ip_v4_data_len, 2, ip_v4_len);
    if (ip_v4_len_res < 0)
    {
        std::cout << "解析 " << ip_v4_data_len << " 失败" << std::endl;
    }
    char ip_v4_head[20] = {
        0b01000101, 0, ip_v4_len[0], ip_v4_len[1], 0, 0, 0b01000000, 0, 0b00010001, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0};
    std::memcpy(ip_v4_head + 12, service_data.ip_src, 4);

    std::memcpy(ip_v4_head + 16, service_data.ip_dst, 4);

    int ip_check_sum_res = computeCheckSum(ip_v4_head, 20, ip_v4_head[10], ip_v4_head[11]);

    if (ip_check_sum_res < 0)
    {
        std::cout << "计算 ip 校验和 失败" << std::endl;
    }

    std::memcpy(buffer + index, ip_v4_head, 20);
    index += 20;

    std::memcpy(buffer + index, service_data.udp_port_src, 2);
    index += 2;
    std::memcpy(buffer + index, service_data.udp_port_dst, 2);
    index += 2;

    std::vector<uint8_t> udp_len;
    int udp_len_res = numberToBytes(udp_data_len, 2, udp_len);
    if (udp_len_res < 0)
    {
        std::cout << "解析udp长度失败" << std::endl;
    }

    buffer[index++] = udp_len[0];
    buffer[index++] = udp_len[1];

    buffer[index++] = 0;
    buffer[index++] = 0;

    std::memcpy(buffer + index, data.data(), data.size());

    index += data.size();

    char udp_check_sum[2];
    int udp_check_sum_res = computeCheckSum(ip_v4_head, index, udp_check_sum[0], udp_check_sum[1]);

    if (udp_check_sum_res < 0)
    {
        std::cout << "计算 udp 校验和 失败" << std::endl;
    }

    buffer[40] = udp_check_sum[0];
    buffer[41] = udp_check_sum[1];

    return index;
}



/* // debug 用 */
std::string toHexString(const std::vector<uint8_t>& vec) {
    std::stringstream ss;
    for (auto byte : vec) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    return ss.str();
}
std::string toHexString(const char* data, size_t length) {
    std::stringstream ss;
    for (size_t i = 0; i < length; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(static_cast<uint8_t>(data[i]));
    }
    return ss.str();
}
size_t hexStringToCharArray(const std::string& hexStr, char* charArray) {
    // 确保hexStr的长度是偶数
    if (hexStr.length() % 2 != 0) {
        throw std::invalid_argument("Hex string length must be even.");
    }
    size_t length = hexStr.length() / 2;
    size_t index = 0;

    std::cout << "Processing hex string: " << hexStr << std::endl;

    for (size_t i = 0; i < length; ++i) {
        // 每两位提取一个字节
        std::string byteStr = hexStr.substr(i * 2, 2);

        try {
            // 将16进制字符串转换为整数
            int byte = std::stoi(byteStr, nullptr, 16);
            
            if (index >= length) {
                throw std::out_of_range("Hex string too long for the provided char array.");
            }

            charArray[index] = static_cast<char>(byte);

            // 输出正在写入的字节值
          

            ++index;
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid argument: " << e.what() << std::endl;
            return -1;
        } catch (const std::out_of_range& e) {
            std::cerr << "Out of range: " << e.what() << std::endl;
            return -1;
        }
    }

    std::cout << "Total bytes written: " << index << std::endl;

    return index;  // 返回处理的字节数
}
