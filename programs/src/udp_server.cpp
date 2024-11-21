#include "../include/udp_server.hpp"
#include <sys/socket.h>

using namespace tcp_vs_udp;

UDPServer::UDPServer(const int& port_number) :
    BasicSocket(port_number, SOCK_STREAM) {
}
