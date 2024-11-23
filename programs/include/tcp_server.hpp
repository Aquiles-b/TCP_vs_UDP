#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

#include "basic_server.hpp"
#include <sys/epoll.h>
#include <vector>

namespace tcp_vs_udp {

    class TCPServer;
    typedef void (*request_handler)(TCPServer *ts, int client_socket);

    class TCPServer : public BasicServer {
        private:
            int epoll_fd;
            std::vector<struct epoll_event> events;
        public:
            TCPServer(const std::string& ip_address, const int& port_number);
            ~TCPServer();

            // Read requests from clients. Accept new clients and process connections.
            void process_requests(request_handler accept_new_client, request_handler process_connection);
            void remove_client(const int& client_socket);

        private:
            // Return false on fail
            bool accept_new_connection(request_handler accept_func);
    };
}

#endif // TCP_SERVER_HPP
