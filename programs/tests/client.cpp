#include "../include/tcp_client.hpp"

using namespace tcp_vs_udp;

int main(int argc, char **argv) {
	if (argc < 4) {
		std::cerr << "Correct usage: ./client <server_ipv4_addr> <server_port> <fname>\n";
		exit(1);
	}
	TCPClient c = TCPClient(argv[1], argv[2]);
	c.connectToServer();
	c.download(argv[3]);

	return 0;
}
