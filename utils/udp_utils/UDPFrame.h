#include <iostream>
#include <vector> // 必须包含 vector 头文件

using namespace std;

#ifndef UDPFRAME_H
#define UDPFRAME_H
namespace UTILS
{
    namespace UDPUTILS
    {
        class UDPFrame
        {
        private:
            vector<uint8_t> m_pre_code;
            vector<uint8_t> m_start;
            vector<uint8_t> m_dst_mac;
            vector<uint8_t> m_src_mac;
            vector<uint8_t> m_bus_type;
            vector<uint8_t> m_ip_v4;
            vector<uint8_t> m_udp_header;
            vector<uint8_t> m_raw_data;
            vector<uint8_t> m_fcs;
        };
    }
}

#endif // UDPFrame