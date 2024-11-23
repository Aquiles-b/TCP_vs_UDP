#include <iostream>
#include <sys/socket.h>
#include "../include/tcp_server.hpp"

using namespace tcp_vs_udp;

void accept_client(TCPServer *ts, const int client_socket) {
    std::cout << "New client: " << client_socket << std::endl;

    char msg[] = "Welcomeee!!\n";
    send(client_socket, msg, sizeof(msg)+1, 0);
}

void process_connection(TCPServer *ts, const int client_socket) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    int bytes_read = read(client_socket, buffer, sizeof(buffer));

    if (bytes_read <= 0) {
        ts->remove_client(client_socket);
        return;
    }

    std::cout << std::string(buffer, bytes_read);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <ip address> <port_number>" << std::endl;
        return 1;
    }

    TCPServer *server = new TCPServer(argv[1], std::atoi(argv[2]));

    while (1) {
        server->process_requests(accept_client, process_connection);
    }

    delete server;

    return 0;
}
