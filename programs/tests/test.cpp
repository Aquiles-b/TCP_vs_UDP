#include <iostream>
#include "../include/tcp_server.hpp"

using namespace tcp_vs_udp;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port_number>" << std::endl;
        return 1;
    }

    TCPServer *server = new TCPServer(std::atoi(argv[1]));

    delete server;

    return 0;
}
