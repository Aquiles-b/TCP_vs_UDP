#include "../include/tcp_client.hpp"
#include "../include/udp_client.hpp"
#include <memory>

using namespace tcp_vs_udp;

int main(int argc, char **argv) {
	if (argc < 5) {
		std::cerr << "Correct usage: ./client <server_ipv4_addr> <server_port> <protocol> <fname> <fout>\n";
		exit(1);
	}
	std::unique_ptr<BasicClient> c;
	if (strcmp("tcp", argv[3]) == 0) { 
		c = std::make_unique<TCPClient>(argv[1], argv[2]);
        c->connectToServer();
        c->download(argv[4], argv[5]);
    }
	else {
		c = std::make_unique<UDPClient>(argv[1], argv[2]);
        c->download(argv[4], argv[5]);
	}


	return 0;
}
