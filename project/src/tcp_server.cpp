#include "../include/tcp_server.hpp"

using namespace tcp_vs_udp;

TCPServer::TCPServer(const std::string& ip_address, const int& port_number, const size_t &buffersize) :
    BasicServer(ip_address, port_number, SOCK_STREAM, buffersize) {
}

TCPServer::~TCPServer() {
    for (auto &client : this->client_threads) {
        client.second.join();
    }
}

int setSocketCork(int clientfd, int flag) {
	return setsockopt(clientfd, IPPROTO_TCP, TCP_CORK, &flag, sizeof(flag));
}

bool TCPServer::sendFile(int clientfd, sockaddr_in &caddr, FILE *file) {
	size_t bytesread;
	size_t &bsize = this->buffersize, fsize;
	uint8_t *buffer = new uint8_t[bsize+1];
	if (!buffer) {
		sendError(clientfd, caddr);
		return false;
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
			return false;
		}
	}

	delete[] buffer;
    return true;
}

void TCPServer::handleClient(int clientfd, sockaddr_in clientaddr) {
	char fname[max_fname_size];
	recv(clientfd, fname, sizeof(fname), 0);

	FILE *fp = fopen(fname, "r");
    char *addr = inet_ntoa(clientaddr.sin_addr);
    int port = ntohs(clientaddr.sin_port);

	if (fp == NULL) {
        std::cout << "Client " << addr;
        std::cout << ":" << port << " requested file ";
        std::cout << fname << " but it does not exist." << std::endl;
		sendError(clientfd, clientaddr);
	}
	else {
        std::cout << "Sending file " << fname << " to client ";
        std::cout << addr << ":" << port << std::endl;
        if (!sendFile(clientfd, clientaddr, fp)) {
            std::cout << "Error: Unable to send file " << fname << " to client ";
            std::cout << addr << ":" << port << std::endl;
        }
        else {
            std::cout << "Successfully sent file " << fname << " to client ";
            std::cout << addr << ":" << port << std::endl;
        }
        fclose(fp);
    }

    close(clientfd);
}

void TCPServer::runParallel() {
	while (true) {
		sockaddr_in clientaddr;
		socklen_t slen = sizeof(clientaddr);
		int clientfd = accept(this->listen_socket, (sockaddr *) &clientaddr, &slen);
		if (clientfd < 0) {
			std::cerr << "Accept error." << std::endl;
			continue;
		}
        std::cout << "Client " << inet_ntoa(clientaddr.sin_addr);
        std::cout << ":" << ntohs(clientaddr.sin_port) << " connected." << std::endl;

		this->client_threadlock.lock();
		this->client_threads[clientfd] = std::thread([this, clientfd, clientaddr]() {
    		this->handleClient(clientfd, clientaddr);
            std::cout << "Client " << inet_ntoa(clientaddr.sin_addr);
            std::cout << ":" << ntohs(clientaddr.sin_port) << " disconnected." << std::endl;
			this->client_threadlock.lock();
			this->client_threads.erase(clientfd);
			this->client_threadlock.unlock();
		});
		this->client_threadlock.unlock();
		this->client_threads[clientfd].detach();
	}
}

void TCPServer::runIterative() {
	while (true) {
		sockaddr_in clientaddr;
		socklen_t slen = sizeof(clientaddr);
		int clientfd = accept(this->listen_socket, (sockaddr *) &clientaddr, &slen);
		if (clientfd < 0) {
			std::cerr << "Accept error." << std::endl;
			continue;
        }
        std::cout << "Client " << inet_ntoa(clientaddr.sin_addr);
        std::cout << ":" << ntohs(clientaddr.sin_port) << " connected." << std::endl;

        this->handleClient(clientfd, clientaddr);

        std::cout << "Client " << inet_ntoa(clientaddr.sin_addr);
        std::cout << ":" << ntohs(clientaddr.sin_port) << " disconnected." << std::endl;
    }
}
