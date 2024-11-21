#ifndef UDP_SERVER_HPP
#define UDP_SERVER_HPP

#include "basic_socket.hpp"

namespace tcp_vs_udp {
    class UDPServer : public BasicSocket {
        private:
            int descr_ptr;

        public:
            UDPServer(const int& port_number);

        private:

    };
}

#endif // UDP_SERVER_HPP
