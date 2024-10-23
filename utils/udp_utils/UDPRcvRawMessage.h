#include <iostream>
#include <array>
#include "UDPFrame.h"
using namespace std;

#ifndef UDPRCVRAWMESSAGE_H
#define UDPRCVRAWMESSAGE_H
namespace UTILS
{
    namespace UDPUTILS
    {
        
        class UDPRcvRawMessage
        {
        private:
            uint16_t m_tte_index;
            uint16_t m_sender_tte_index;
            uint16_t m_sender_tte_port;
            uint16_t m_tte_portType;
            uint16_t m_data_type;
            uint16_t m_dst_vlid;
            uint16_t m_src_vlid;
            uint16_t m_data_len;
            uint16_t m_data_time;
            string m_ext_info;
            UDPFrame m_data;

        public:
        };

        
    }
}
#endif