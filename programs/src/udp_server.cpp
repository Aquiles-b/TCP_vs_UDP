#include "../include/udp_server.hpp"
#include <sys/socket.h>

using namespace udp_server;

UDPServer::UDPServer(const int& port_number) :
    basic_socket::BasicSocket(port_number, SOCK_STREAM) {


}
