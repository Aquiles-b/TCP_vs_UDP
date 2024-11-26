#include "../include/tcp_client.hpp"
#include "../include/udp_client.hpp"
#include <memory>

using namespace tcp_vs_udp;

int main(int argc, char **argv) {
	if (argc < 5) {
		std::cerr << "Correct usage: ./client <server_ipv4_addr> <server_port> <fname> <protocol>\n";
		exit(1);
	}
	std::unique_ptr<BasicClient> c;
	if (strcmp("tcp", argv[4]) == 0)
		c = std::make_unique<TCPClient>(argv[1], argv[2]);
	else {
		c = std::make_unique<UDPClient>(argv[1], argv[2]);
	}

	c->connectToServer();
	c->download(argv[3]);

	return 0;
}
