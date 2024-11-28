#ifndef BASIC_SERVER_HPP
#define BASIC_SERVER_HPP
#include <iostream>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "protocol.h"

namespace tcp_vs_udp {
    class BasicServer {
        protected:
            int listen_socket;
            struct sockaddr_in listen_socket_addr;
			size_t buffersize;

			virtual bool sendFile(int clientfd, sockaddr_in &caddr, FILE *file);
			int sendError(int clientfd, sockaddr_in &caddr) const;

        public:
            BasicServer(const std::string& ip_address, const int& port_number, const int& socket_type,
                        const size_t &buffersize);
            ~BasicServer();

			void setBufferSize(size_t buffersize);

    };
}

#endif // BASIC_SERVER_HPP
