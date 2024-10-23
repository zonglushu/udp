#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::udp;

class UDPClient
{
public:
    UDPClient(boost::asio::io_context &io_context, const std::string &host, short port)
        : socket_(io_context), endpoint_(boost::asio::ip::make_address(host), port)
    {
        socket_.open(udp::v4());
    }

    void send(const std::string &message)
    {
        socket_.send_to(boost::asio::buffer(message), endpoint_);
        receive();
    }

private:
    void receive()
    {
        std::array<char, 1024> recv_buffer;
        udp::endpoint sender_endpoint;
        size_t len = socket_.receive_from(boost::asio::buffer(recv_buffer), sender_endpoint);
        std::cout << "Server response: " << std::string(recv_buffer.data(), len) << std::endl;
    }

    udp::socket socket_;
    udp::endpoint endpoint_;
};

int main()
{
    try
    {
        boost::asio::io_context io_context;
        UDPClient client(io_context, "127.0.0.1", 12345);
        client.send("Hello from client!");
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
