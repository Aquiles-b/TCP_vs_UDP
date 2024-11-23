#include "../include/basic_server.hpp"

using namespace tcp_vs_udp;

BasicServer::BasicServer(const std::string& ip_address, const int& port_number, const int& socket_type)
	: buffersize{1024} {
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

    if (listen(this->listen_socket, 5) < 0) {
        std::cerr << "Error listening on the port " << port_number << std::endl;
        exit(1);
	}
}

BasicServer::~BasicServer() {
    close(this->listen_socket);
    std::cout << "Socket closed." << std::endl;
}

void BasicServer::handleClientIterative(int clientfd, sockaddr_in clientaddr) {
	std::cout << "Yaaaaaaaaaaay!\n";
}

void BasicServer::handleClientParallel(int clientfd, sockaddr_in clientaddr) {
	std::cout << "Yaaaaaaaaaaay!\n";
}

void BasicServer::sendFile(int clientfd, sockaddr_in &caddr, FILE *file) {
	std::cout << "Wooooooooooow!\n";
}

void BasicServer::runParallel() {
	while (true) {
		sockaddr_in clientaddr;
		socklen_t slen = sizeof(clientaddr);
		int clientfd = accept(this->listen_socket, (sockaddr *) &clientaddr, &slen);
		if (clientfd < 0) {
			std::cerr << "Accept error." << std::endl;
			continue;
		}
		this->client_threadlock.lock();
		this->client_threads[clientfd] = std::thread([this, clientfd, clientaddr]() {
    		this->handleClientParallel(clientfd, clientaddr);
		});
		this->client_threadlock.unlock();
		this->client_threads[clientfd].detach();
	}
}

int BasicServer::sendError(int clientfd, sockaddr_in &caddr) const {
	uint8_t error = ERROR;
	ssize_t ret;
	ret = sendto(clientfd, &error, sizeof(error), 0, (sockaddr *) &caddr, sizeof(caddr));
	
	return ret;
}

void BasicServer::setBufferSize(size_t buffersize) {
	this->buffersize = buffersize;
}
