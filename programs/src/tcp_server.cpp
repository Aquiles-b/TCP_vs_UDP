#include "../include/tcp_server.hpp"
#include <iostream>
#include <unistd.h>
#include <cstring>

using namespace tcp_vs_udp;

TCPServer::TCPServer(const std::string& ip_address, const int& port_number) :
    BasicServer(ip_address, port_number, SOCK_STREAM), epoll_fd{-1} {

    this->epoll_fd = epoll_create1(0);
    if (this->epoll_fd < 0) {
        std::cerr << "Error creating epoll: " << strerror(errno) << std::endl;
        exit(1);
    }

    // Configura o socket do servidor para o epoll
    struct epoll_event event;
    event.events = EPOLLIN; // Interesse em leitura
    event.data.fd = this->listen_socket;

    if (epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, this->listen_socket, &event) < 0) {
        std::cerr << "Error adding server socket on epoll: " << strerror(errno) << std::endl;
        close(this->epoll_fd);
        exit(1);
    }

    if (listen(this->listen_socket, 5) < 0) {
        std::cerr << "Error listening on the port " << port_number << std::endl;
        exit(1);
    }

    this->events.resize(10);

    std::cout << "Socket opened." << std::endl;
}

TCPServer::~TCPServer() {
    if (this->epoll_fd >= 0) {
        close(this->epoll_fd);
    }
}

void TCPServer::process_requests(request_handler accept_new_client, request_handler process_connection) {
    int event_count = epoll_wait(this->epoll_fd, this->events.data(), this->events.size(), -1);
    if (event_count < 0) {
        std::cerr << "Error: epoll_wait: " << strerror(errno) << std::endl;
        return;
    }
    for (int i = 0; i < event_count; i++) {
        int event_fd = this->events[i].data.fd;

        if (event_fd == this->listen_socket) {
            this->accept_new_connection(accept_new_client);
        } 
        else if (this->events[i].events & EPOLLIN) {
            if (process_connection) {
                process_connection(this, event_fd);
            }
            
        }
    }
}

void TCPServer::remove_client(const int& client_socket) {
    epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, client_socket, nullptr);
    close(client_socket);
}

bool TCPServer::accept_new_connection(request_handler accept_func) {
    int new_client_socket = accept(this->listen_socket, nullptr, nullptr);
    if (new_client_socket < 0) {
        std::cerr << "Error accepting new connection: " << strerror(errno) << std::endl;
        return false;
    }

    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET; // Interesse em leitura com edge-triggered
    event.data.fd = new_client_socket;

    if (epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, new_client_socket, &event) < 0) {
        std::cerr << "Error adding new client on epoll: " << strerror(errno) << std::endl;
        close(new_client_socket);
        return false;
    }

    if (accept_func) {
        accept_func(this, new_client_socket);
    }

    return true;
}
