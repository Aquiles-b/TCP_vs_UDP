#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

#include "basic_socket.hpp"

namespace tcp_vs_udp {
    class TCPServer : public BasicSocket {
        private:
        public:
            TCPServer(const int& port_number);
    };
}

#endif // TCP_SERVER_HPP
