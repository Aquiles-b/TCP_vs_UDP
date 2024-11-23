#ifndef UDP_SERVER_HPP
#define UDP_SERVER_HPP

#include "basic_server.hpp"

namespace tcp_vs_udp {
    class UDPServer : public BasicServer {
        private:

        public:
            UDPServer(const std::string& ip_address, const int& port_number);

        private:

    };
}

#endif // UDP_SERVER_HPP
