#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

#include "basic_server.hpp"
#include <sys/epoll.h>
#include <netinet/tcp.h>
#include <thread>
#include <mutex>
#include <unordered_map>

namespace tcp_vs_udp {
    class TCPServer : public BasicServer {
        private:
			// Mapeia descritor de cliente para uma thread, caso esteja usando o servidor paralelo
			std::unordered_map<int, std::thread> client_threads;
			std::mutex client_threadlock;

        public:
            TCPServer(const std::string& ip_address, const int& port_number, const size_t &buffersize);
            ~TCPServer();

			void runParallel();
			void runIterative();

		protected:
			void handleClient(int clientfd, sockaddr_in clientaddr);
			virtual bool sendFile(int clientfd, sockaddr_in &caddr, FILE *file) override;
    };
}

#endif // TCP_SERVER_HPP
