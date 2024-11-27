#include "../include/tcp_server.hpp"
#include <iostream>
#include <unistd.h>
#include <cstring>

using namespace tcp_vs_udp;

TCPServer::TCPServer(const std::string& ip_address, const int& port_number) :
    BasicServer(ip_address, port_number, SOCK_STREAM) {

    std::cout << "Socket opened." << std::endl;
}

int setSocketCork(int clientfd, int flag) {
	return setsockopt(clientfd, IPPROTO_TCP, TCP_CORK, &flag, sizeof(flag));
}

void TCPServer::sendFile(int clientfd, sockaddr_in &caddr, FILE *file) {
	size_t bytesread;
	size_t &bsize = this->buffersize, fsize;
	uint8_t *buffer = new uint8_t[bsize+1];
	if (!buffer) {
		sendError(clientfd, caddr);
		return;
	}
	buffer[0] = MessageType::TXDATA;
	fseek(file, 0, SEEK_END); // Fim do arquivo
	fsize = ftell(file); // Pega o tamanho
	((size_t*) (buffer+1))[0] = fsize; // Coloca tamanho na mensagem
	((size_t*) (buffer+1))[1] = bsize; // Coloca tamanho do buffer na mensagem
	// Mensagem:
	// MTYPE (uint8) | FILE SIZE (size_t) | SERVER TRANSMISSION BUFFER SIZE (size_t)
	send(clientfd, buffer, 1+2*sizeof(size_t), 0);
	fseek(file, 0, SEEK_SET);
	while ((bytesread = fread(buffer, 1, bsize, file)) > 0) {
		if (send(clientfd, buffer, bytesread, MSG_MORE) < 0) {
			delete[] buffer;
			return;
		}
	}

	delete[] buffer;
}

void TCPServer::handleClient(int clientfd, sockaddr_in clientaddr) {
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
}
