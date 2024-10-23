#include <boost/asio.hpp>
#include <iostream>
#include <array>
#include "UDPRcvRawMessage.h"
#include "UDPCardState.h"
#include "UDPServiceState.h"
using namespace std;
using boost::asio::ip::udp;
namespace UTILS
{
    namespace UDPUTILS
    {
        // 接收到数据的回调函数接口
        typedef void (*DataCallback)(UDPRcvRawMessage &data);
        // 接收到TTE板卡发出的板卡状态数据时的回调函数
        typedef void (*CardStateCallback)(UDPCardState &cardstate);

        // 接收到TTE板卡发出的驱动服务状态数据时的回调函数
        typedef void (*ServiceStateCallback)(UDPServiceState &cardstate_list);

        class UDPAccess
        {
        public:
            UDPAccess(boost::asio::io_context &io_context, short port)
                : socket_(io_context, udp::endpoint(udp::v4(), port))
            {
                startReceive();
            }

            void startReceive();
            void sendResponse();

            // 1.注册TTE板卡接收到数据的回调函数的接口
            void RegisterDataCallback(DataCallback callback);

            // 2.注册接收到TTE板卡发出的板卡状态数据时的回调函数
            void RegisterCardStateCallback(CardStateCallback callback);

            // 3.注册接收到TTE板卡发出的驱动服务状态数据时的回调函数
            void RegisterServiceStateCallback(ServiceStateCallback callback);

            // 4.板卡初始化接口
            int init_board(const char *localip, const char *groupip, uint16_t groupport);

            // 5.设置工作模式接口
            // tte_index:板卡序号
            // mode： 1:监控模式，2：回灌对接模式，3：周期对接模式
            // card1：监控模式的第一块监控卡
            // Card2：监控模式的第二块监控卡
            int set_mode(int tte_index, int mode, int card1 = 0, int card2 = 0);

            // 6.设置冗余模式接口
            // mode：0:三冗余，1：双冗余
            int set_redunmode(int tte_index, int mode);

            // 7.设置数据操作模式接口
            // mode：0:单帧寄存器，1：多帧寄存器，2：轮询dma
            int set_dmamode(int tte_index, int mode);

            /*-
            //19.函数功能: 发送数据
            Description   发送数据
            Parameter
            @ tte_index        : 板卡ID，从0开始
            @ vl_id  :  虚拟链路值
            @ data_type    : 数据类型，1-BE，2-RC，3-其他未明确-TT(认为是TT_CMD数据)
            * 返回值   : 成功=0、 失败=非0
            -*/
            int start_send(int tte_index, int vl_id, int data_type, char *data_buff, int data_len);

            // 20.数据表加载，1-业务规划表，2-周期数据表，3-回灌数据文件，4-IP端口信息
            int load_config(int tte_index, int type);

            // 21.退出
            void quit();




        private:
            // void startReceive()
            // {
            //     socket_.async_receive_from(
            //         boost::asio::buffer(recv_buffer_), remote_endpoint_,
            //         [this](boost::system::error_code ec, std::size_t bytes_recvd)
            //         {
            //             if (!ec && bytes_recvd > 0)
            //             {
            //                 std::cout << "Received: " << std::string(recv_buffer_.data(), bytes_recvd) << std::endl;
            //                 sendResponse();
            //             }
            //             startReceive(); // 继续接收下一个数据包
            //         });
            // }

            // void sendResponse()
            // {
            //     std::string message = "Hello from server!";
            //     socket_.async_send_to(
            //         boost::asio::buffer(message), remote_endpoint_,
            //         [](boost::system::error_code /*ec*/, std::size_t /*bytes_sent*/) {});
            // }

            udp::socket socket_;                 // UDP 套接字
            udp::endpoint remote_endpoint_;      // 远程客户端的地址信息
            std::array<char, 1024> recv_buffer_; // 用于存储接收到的数据
        };
    }
}