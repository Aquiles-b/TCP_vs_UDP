#include "../include/tcp_server.hpp"

using namespace tcp_server;

TCPServer::TCPServer(const int& port_number) : 
    basic_socket::BasicSocket(port_number, SOCK_STREAM) {

    if (listen(this->descr_ptr, 5) < 0) {
        std::cerr << "Error listening on port " << port_number << std::endl;
        exit(1);
    }

    std::cout << "Socket opened." << std::endl;
}
