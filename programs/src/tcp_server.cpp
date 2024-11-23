#include "../include/tcp_server.hpp"
#include <iostream>
#include <unistd.h>
#include <cstring>

using namespace tcp_vs_udp;

TCPServer::TCPServer(const std::string& ip_address, const int& port_number) :
    BasicServer(ip_address, port_number, SOCK_STREAM) {

    std::cout << "Socket opened." << std::endl;
}

void TCPServer::sendFile(int clientfd, sockaddr_in &caddr, FILE *file) {
	size_t bytesread;
	size_t &bsize = this->buffersize;
	uint8_t *buffer = new uint8_t[bsize+1];
	if (!buffer) {
		sendError(clientfd, caddr);
		return;
	}
	buffer[0] = MessageType::DATA;
	while ((bytesread = fread(buffer+1, 1, bsize, file)) > 0) {
		if (send(clientfd, buffer, bsize+1, 0) < 0) {
			delete buffer;
			return;
		}
	}

	delete buffer;
}

void TCPServer::handleClientParallel(int clientfd, sockaddr_in clientaddr) {
	
	char fname[256];
	recv(clientfd, fname, sizeof(fname), 0);
	FILE *fp = fopen(fname, "r");
	if (fp == NULL) {
		sendError(clientfd, clientaddr);
	}
	else {
		sendFile(clientfd, clientaddr, fp);
		fclose(fp);
	}

	close(clientfd);
	this->client_threadlock.lock();
	this->client_threads.erase(clientfd);
	this->client_threadlock.unlock();
}
