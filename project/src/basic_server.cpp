#include "../include/basic_server.hpp"

using namespace tcp_vs_udp;

BasicServer::BasicServer(const std::string& ip_address, const int& port_number, const int& socket_type,
            const size_t &buffersize)
	: buffersize{buffersize} {
    this->listen_socket = socket(AF_INET, socket_type, 0);
    if (this->listen_socket < 0) {
        std::cerr << "Error: Unable to open socket." << std::endl;
        exit(1);
    }
	int flag = 1;
	setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

    // Initialize Address 
    this->listen_socket_addr.sin_family = AF_INET;
    this->listen_socket_addr.sin_port = htons(port_number);
	//this->listen_socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    this->listen_socket_addr.sin_addr.s_addr = inet_addr(ip_address.c_str()); 
    std::memset(&(this->listen_socket_addr.sin_zero), 0, 8);

    // Bind
    if (bind(this->listen_socket, (sockaddr*) &this->listen_socket_addr, sizeof(sockaddr)) < 0) {
        std::cerr << "Error: Fail to bind on local port." << std::endl;
        exit(1);
    }

    if (socket_type == SOCK_STREAM) {
        if (listen(this->listen_socket, 5) < 0) {
            std::cerr << "Error listening on the port " << port_number << std::endl;
            exit(1);
        }
    }
    std::cout << "Server opened on port " << port_number << std::endl;
    std::cout << "Buffer size: " << this->buffersize << std::endl;
}

BasicServer::~BasicServer() {
    close(this->listen_socket);
    std::cout << "Server closed." << std::endl;
}

bool BasicServer::sendFile(int clientfd, sockaddr_in &caddr, FILE *file) {
    return false;
}

int BasicServer::sendError(int clientfd, sockaddr_in &caddr) const {
    uint8_t error = MessageType::ERROR;
    ssize_t ret;
    ret = sendto(clientfd, &error, sizeof(error), 0, (sockaddr *) &caddr, sizeof(caddr));

    return ret;
}

void BasicServer::setBufferSize(size_t buffersize) {
    this->buffersize = buffersize;
}
