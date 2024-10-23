#include <boost/asio.hpp>
#include <iostream>
#include <array>

using boost::asio::ip::udp;

class UDPServer
{
public:
    UDPServer(boost::asio::io_context &io_context, short port)
        : socket_(io_context, udp::endpoint(udp::v4(), port))
    {
        startReceive();
    }

private:
    void startReceive()
    {
        socket_.async_receive_from(
            boost::asio::buffer(recv_buffer_), remote_endpoint_,
            [this](boost::system::error_code ec, std::size_t bytes_recvd)
            {
                if (!ec && bytes_recvd > 0)
                {
                    std::cout << "Received: " << std::string(recv_buffer_.data(), bytes_recvd) << std::endl;
                    sendResponse();
                }
                startReceive();
            });
    }

    void sendResponse()
    {
        std::string message = "Hello from server!";
        socket_.async_send_to(
            boost::asio::buffer(message), remote_endpoint_,
            [](boost::system::error_code /*ec*/, std::size_t /*bytes_sent*/) {});
    }

    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    std::array<char, 1024> recv_buffer_;
};

int main()
{
    try
    {
        boost::asio::io_context io_context;
        UDPServer server(io_context, 12345);
        io_context.run();
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
