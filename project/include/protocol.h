#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_
#include <cstddef>

namespace tcp_vs_udp {
    constexpr size_t max_fname_size = 256;

	enum MessageType {
		ERROR,
		DATA,
		ACK,
		NACK,
		ENDTX,
		TXDATA,
	};
}

#endif
