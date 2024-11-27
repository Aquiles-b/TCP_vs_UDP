#ifndef UDP_CLIENT_HPP
#define UDP_CLIENT_HPP

#include "basic_client.hpp"

namespace tcp_vs_udp {
	class UDPClient : public BasicClient {
        private:
            int win_size;
            int init_win_idx;
            uint8_t *sliding_win_buf;
            size_t bsize;
            size_t last_message_size;
            uint8_t current_seq;
		public:
			UDPClient(char *addrstr, char *port);
			virtual int download(char *fname, char *fout) override;

            bool get_buffer_and_win_size(char *fname);

            void setWinSize(int win_size);
            int recv_window(uint8_t **sliding_win);
            void init_sliding_win_buf();

            void send_ACK() const;
            void send_NACK() const;
	};
}

#endif
