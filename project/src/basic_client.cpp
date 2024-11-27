#include "../include/basic_client.hpp"

using namespace tcp_vs_udp;

MessageType BasicClient::getMessageType(uint8_t *buffer) {
	return static_cast<MessageType>(buffer[0]);
}

BasicClient::BasicClient(int protocol, char *addrstr, char *port) {
	int pnum;
	memset(&this->saddr, 0, sizeof(this->saddr));
	saddr.sin_family = AF_INET;
	if (inet_pton(saddr.sin_family, addrstr, &saddr.sin_addr) < 0) {
		std::cerr << "Invalid address.\n";
		exit(1);
	}
	if ((pnum = std::atoi(port)) == 0 || pnum >= 1 << 16) {
		std::cerr << "Invalid port number.\n";
		exit(1);
	}
	saddr.sin_port = htons(pnum);
	if ((sock = socket(AF_INET, protocol, 0)) < 0) {
		std::cerr << "Error at socket creation.\n";
		perror("");
		exit(1);
	}
}

BasicClient::~BasicClient() {
	close(sock);
}

void BasicClient::connectToServer() {
	if (connect(sock, (sockaddr *) &saddr, sizeof(saddr)) < 0) {
		std::cerr << "Error at connect.\n";
		perror("");
		exit(1);
	}
}

int BasicClient::download(char *fname, char *fout) {
	std::cout << "Placeholder!\n";
	return 1;
}
