#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

constexpr size_t max_fname_size = 256;

namespace tcp_vs_udp {
	enum MessageType {
		ERROR,
		DATA,
		DOWNLOAD,
		ACK,
		NACK,
		ENDTX,
		TXDATA,
	};
}

#endif
