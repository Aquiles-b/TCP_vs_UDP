#ifndef BASIC_SERVER_HPP
#define BASIC_SERVER_HPP
#include <iostream>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

namespace tcp_vs_udp {
    class BasicServer {
        protected:
            int listen_socket;
            struct sockaddr_in listen_socket_addr;
        public:
            BasicServer(const std::string& ip_address, const int& port_number, const int& socket_type);
            ~BasicServer();
    };
}

#endif // BASIC_SERVER_HPP
