#ifndef UDP_SERVER_HPP
#define UDP_SERVER_HPP

#include "basic_server.hpp"

namespace tcp_vs_udp {
    class UDPServer : public BasicServer {
        private:
            int win_size;

        public:
            UDPServer(const std::string& ip_address, const int& port_number);
            UDPServer(const std::string& ip_address, const int& port_number, const int& win_size);

			virtual void handleClient(int clientfd, sockaddr_in clientaddr) override;
			virtual void sendFile(int clientfd, sockaddr_in &caddr, FILE *file) override;

            bool send_window(const int&clientfd, uint8_t *sliding_window, const sockaddr_in &caddr,
                                                const int &win_init_idx, const size_t &tam_last_message);

            int fill_sliding_window(uint8_t *sliding_window, FILE *file, const int &win_init_idx, 
                                    const int &num_new_messages, uint8_t &current_seq, size_t &tam_last_message);

            bool send_file_and_buffer_info(int clientfd, sockaddr_in &caddr, FILE *file, const size_t &bsize);

            void setWinSize(size_t win_size);

        private:

    };
}

#endif // UDP_SERVER_HPP
