#include "../include/basic_server.hpp"

using namespace tcp_vs_udp;

BasicServer::BasicServer(const std::string& ip_address, const int& port_number, const int& socket_type) {
    this->listen_socket = socket(AF_INET, socket_type, 0);
    if (this->listen_socket < 0) {
        std::cerr << "Error: Unable to open socket." << std::endl;
        exit(1);
    }

    // Initialize Address 
    this->listen_socket_addr.sin_family = AF_INET;
    this->listen_socket_addr.sin_port = htons(port_number);
    this->listen_socket_addr.sin_addr.s_addr = inet_addr(ip_address.c_str()); 
    std::memset(&(this->listen_socket_addr.sin_zero), 0, 8);

    // Bind
    if (bind(this->listen_socket, (sockaddr*) &this->listen_socket_addr, sizeof(sockaddr)) < 0) {
        std::cerr << "Error: Fail to bind on local port." << std::endl;
        exit(1);
    }
}

BasicServer::~BasicServer() {
    close(this->listen_socket);
    std::cout << "Socket closed." << std::endl;
}
