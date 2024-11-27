#include "../include/tcp_server.hpp"
#include "../include/udp_server.hpp"
#include <memory>

using namespace tcp_vs_udp;

int main(int argc, char **argv) {
	if (argc < 4) {
		std::cerr << "Correct usage: ./server <ipv4_addr> <port> <protocol> <win_size (udp)>\n";
		exit(1);
	}
	std::unique_ptr<BasicServer> c;
	if (strcmp("tcp", argv[3]) == 0)
		c = std::make_unique<TCPServer>(argv[1], std::atoi(argv[2]));
	else {
		c = std::make_unique<UDPServer>(argv[1], std::atoi(argv[2]), std::atoi(argv[4]));
	}

    c->setBufferSize(8192);
    c->runParallel();

	return 0;
}
