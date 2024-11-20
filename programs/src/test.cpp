#include <iostream>
#include "../include/tcp_server.hpp"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port_number>" << std::endl;
        return 1;
    }

    tcp_server::TCPServer *server = new tcp_server::TCPServer(std::atoi(argv[1]));

    delete server;

    return 0;
}
