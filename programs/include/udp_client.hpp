#ifndef UDP_CLIENT_HPP
#define UDP_CLIENT_HPP

#include "basic_client.hpp"

namespace tcp_vs_udp {
	class UDPClient : public BasicClient {
		public:
			UDPClient(char *addrstr, char *port);
			virtual int download(char *fname) override;

            size_t get_buffer_size(char *fname);
	};
};

#endif
