#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

namespace tcp_vs_udp {
	enum MessageType {
		ERROR,
		DATA,
		DOWNLOAD,
		ACK,
		NACK,
		ENDTX,
	};
}

#endif
