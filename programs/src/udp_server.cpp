#include "../include/udp_server.hpp"
#include <sys/socket.h>

using namespace tcp_vs_udp;

UDPServer::UDPServer(const std::string& ip_address, const int& port_number) :
    BasicServer(ip_address, port_number, SOCK_STREAM) {
}
