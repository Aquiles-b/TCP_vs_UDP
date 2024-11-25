#ifndef UDP_SERVER_HPP
#define UDP_SERVER_HPP

#include "basic_server.hpp"

namespace tcp_vs_udp {
    class UDPServer : public BasicServer {
        private:
            int win_size;

        public:
            UDPServer(const std::string& ip_address, const int& port_number);

			virtual void handleClient(int clientfd, sockaddr_in clientaddr) override;
			virtual void sendFile(int clientfd, sockaddr_in &caddr, FILE *file) override;

            bool send_window(const int&clientfd, u_int8_t *sliding_window, const sockaddr_in &caddr,
                                                const int &win_init_idx, const size_t &tam_last_message);

            int fill_sliding_window(u_int8_t *sliding_window, FILE *file, const int &win_init_idx, 
                                    const int &num_new_messages, u_int8_t &current_seq, size_t &tam_last_message);

            void setWinSize(size_t win_size);

        private:

    };
}

#endif // UDP_SERVER_HPP
