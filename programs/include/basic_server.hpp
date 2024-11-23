#ifndef BASIC_SERVER_HPP
#define BASIC_SERVER_HPP
#include <iostream>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <unordered_map>
#include "protocol.h"

namespace tcp_vs_udp {
    class BasicServer {
        protected:
            int listen_socket;
            struct sockaddr_in listen_socket_addr;
			// Mapeia descritor de cliente para uma thread, caso esteja usando o servidor paralelo
			std::unordered_map<int, std::thread> client_threads;
			std::mutex client_threadlock;
			size_t buffersize;

			virtual void handleClientIterative(int clientfd, sockaddr_in clientaddr);

			virtual void handleClientParallel(int clientfd, sockaddr_in clientaddr);

			virtual void sendFile(int clientfd, sockaddr_in &caddr, FILE *file);

			int sendError(int clientfd, sockaddr_in &caddr) const;

        public:
            BasicServer(const std::string& ip_address, const int& port_number, const int& socket_type);
            ~BasicServer();

			void runParallel();

			void runIterative();

			void setBufferSize(size_t buffersize);

    };
}

#endif // BASIC_SERVER_HPP
