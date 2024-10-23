#include <boost/asio.hpp>
#include <iostream>
#include <array>

using namespace std;
using boost::asio::ip::udp;

namespace UTILS
{
    namespace UDPUTILS
    {
        class UDPServiceState
        {
            private:
                uint32_t m_tte_time;
                uint16_t m_state;
                uint16_t m_work_mode;
                uint32_t m_recv_queue_number;
                uint32_t m_send_queue_number;
        };

    }
}