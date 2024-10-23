#include <iostream>
#include <array>

using namespace std;

namespace UTILS
{
    namespace UDPUTILS
    {
        class UDPCardStateElement
        {
            private:
                uint16_t m_udp_index;
                uint16_t m_state;
                uint16_t m_work_mode;
                uint16_t m_schedule_table_length;
                uint16_t m_tt_table_length;
                uint16_t m_tt_vl_number;
                uint16_t m_sample_table_length;
                uint16_t m_sample_vl_number;
                std::string m_ip;
                uint16_t m_port;
                uint16_t m_pci_tte_present;
                uint32_t m_pci_device_id;
        };

        class UDPCardState
        {
            private:
                uint32_t m_state_time;
                array<UDPCardStateElement, 8> m_card;
        };
    }
}