#ifndef UDP_SERVER_HPP
#define UDP_SERVER_HPP

#include "basic_socket.hpp"

namespace udp_server {
    class UDPServer : public basic_socket::BasicSocket {
        private:
            int descr_ptr;

        public:
            UDPServer(const int& port_number);

        private:

    };
}

#endif // UDP_SERVER_HPP
