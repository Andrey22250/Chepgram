#include <boost\asio.hpp>
#include <memory>
#include <string>

class TcpServer {
public:
    TcpServer(boost::asio::io_context& io_context, short port);

private:
    void doAccept();

    boost::asio::ip::tcp::acceptor acceptor_;
};