#include <iostream>

#include <asio.hpp>

int main(int argc, char* argv[])
{
    //asio listen tcp ip=127.0.0.1 port 51771
    asio::io_context io_context;
    asio::ip::tcp::acceptor acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 51771));
    asio::ip::tcp::socket socket(io_context);
    acceptor.accept(socket);
    std::cout << "Connected" << std::endl;
    return 0;
}