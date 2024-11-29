#include "../include/tcp_server.hpp"
#include "../include/udp_server.hpp"
#include <memory>

using namespace tcp_vs_udp;

int main(int argc, char **argv) {
	std::cout << ">";
	for (int i = 0; i < argc; i++) {
		std::cout << " " << argv[i];
	}
	std::cout << "\n";
	if (argc != 6) {
		std::cerr << "Correct usage: ./server <ipv4_addr> <port> <protocol> <buffer_size> <win_size_udp | mode_tcp(iter|par)>\n";
		exit(1);
	}
	if (strcmp("tcp", argv[3]) == 0) {
        std::unique_ptr<TCPServer> c;
		c = std::make_unique<TCPServer>(argv[1], std::atoi(argv[2]), std::atoi(argv[4]));
        if (strcmp("iter", argv[5]) == 0) {
            c->runIterative();
        }
        else if (strcmp("par", argv[5]) == 0) {
            c->runParallel();
        }
        else {
            std::cerr << argv[5] << " is not a valid mode for TCP. Use 'iter' or 'par'.\n";
            exit(1);
        }
    }
	else {
        std::unique_ptr<UDPServer> c;
        if (strcmp("iter", argv[5]) == 0 || strcmp("par", argv[5]) == 0) {
            std::cerr << argv[5] << " is not a valid mode for UDP. Use a window size instead.\n";
            exit(1);
        }
		c = std::make_unique<UDPServer>(argv[1], std::atoi(argv[2]), std::atoi(argv[4]), std::atoi(argv[5]));
        c->runIterative();
	}

	return 0;
}
