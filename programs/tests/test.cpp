#include <iostream>
#include <sys/socket.h>
#include "../include/tcp_server.hpp"

using namespace tcp_vs_udp;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <ip address> <port_number>" << std::endl;
        return 1;
    }

    while (1) {
    }

    return 0;
}
