#include "../include/udp_client.hpp"

using namespace tcp_vs_udp;

MessageType getMessageType(uint8_t *buffer) {
	return static_cast<MessageType>(buffer[0]);
}

UDPClient::UDPClient(char *addrstr, char *port) 
	: BasicClient(SOCK_STREAM, addrstr, port) {

}

int UDPClient::download(char *fname) {
	FILE *f = fopen(fname, "w");
	if (!f) {
		std::cerr << "Error at opening file.\n";
		return 0;
	}
	size_t bsize = get_buffer_size(fname);
    if (bsize == 0) {
        std::cerr << "Error: get_buffer_size: " << errno << std::endl;
        return 0;
    }

	return 1;
}

size_t UDPClient::get_buffer_size(char *fname) {
	uint8_t txdatabuffer[1+2*sizeof(size_t)], *dlbuffer;
	ssize_t bytesread;
	size_t serverbuffersize, fsize, bsize;
	MessageType mtype;
	bool endtx = false;
	if (sendto(sock, fname, strnlen(fname, max_fname_size) + 1, 0, 
                                    (sockaddr *) &saddr, sizeof(saddr)) < 0) {
        std::cerr << "Error: sendto: " << errno << std::endl;
    }
    socklen_t saddr_len = sizeof(saddr);
	if ((bytesread = recvfrom(sock, txdatabuffer, sizeof(txdatabuffer), 0,
                                        (sockaddr *) &saddr, &saddr_len) < 0)) {
		return 0;
	}
	else {
		mtype = getMessageType(txdatabuffer);
		switch (mtype) {
		case TXDATA:
			fsize = ((size_t *) (txdatabuffer+1))[0];
			(void) fsize; // Remove warning de variavel nao utilizada
			return ((size_t *) (txdatabuffer+1))[1];
			break;
		case ERROR:
			std::cerr << "Server error ocurred.\n";
			return 0;
			break;
		default:
			std::cerr << "Unexpected message received. Terminating connection.\n";
			return 0;
			break;
		}
	}
}
