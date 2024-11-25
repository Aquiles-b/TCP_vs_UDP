#ifndef _TCP_CLIENT_
#define _TCP_CLIENT_

#include "basic_client.hpp"

namespace tcp_vs_udp {
	constexpr size_t bsize = 4096;
	class TCPClient : public BasicClient {
		public:
			TCPClient(char *addrstr, char *port);
			virtual int download(char *fname) override;
	};
};

#endif
