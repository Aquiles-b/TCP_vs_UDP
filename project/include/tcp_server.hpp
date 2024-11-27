#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

#include "basic_server.hpp"
#include <sys/epoll.h>
#include <vector>
#include <netinet/tcp.h>

namespace tcp_vs_udp {
    class TCPServer : public BasicServer {
        private:
        public:
            TCPServer(const std::string& ip_address, const int& port_number);
		protected:
			virtual void handleClient(int clientfd, sockaddr_in clientaddr) override;

			virtual void sendFile(int clientfd, sockaddr_in &caddr, FILE *file) override;

        private:

    };
}

#endif // TCP_SERVER_HPP
