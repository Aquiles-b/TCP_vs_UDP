#include "../include/basic_socket.hpp"

using namespace basic_socket;

BasicSocket::BasicSocket(const int& port_number, const int& socket_type) {
    this->descr_ptr = socket(AF_INET, socket_type, 0);
    if (this->descr_ptr < 0) {
        std::cerr << "Error: Unable to open socket." << std::endl;
        exit(1);
    }

    // Initialize Address 
    this->address.sin_family = AF_INET;
    this->address.sin_port = htons(port_number);
    this->address.sin_addr.s_addr = INADDR_ANY;
    std::memset(&(this->address.sin_zero), 0, 8);

    // Bind
    if (bind(this->descr_ptr, (sockaddr*) &this->address, sizeof(sockaddr)) < 0) {
        std::cerr << "Error: Fail to bind on local port." << std::endl;
        exit(1);
    }
}

BasicSocket::~BasicSocket() {
    close(this->descr_ptr);
    std::cout << "Socket closed." << std::endl;
}
