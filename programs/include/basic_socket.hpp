#ifndef BASIC_SOCKET_HPP
#define BASIC_SOCKET_HPP
#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

namespace basic_socket {
    class BasicSocket {
        protected:
            int descr_ptr;
            struct sockaddr_in address;
        public:
            BasicSocket(const int& port_number, const int& socket_type);
            ~BasicSocket();
    };
}

#endif // BASIC_SOCKET_HPP
