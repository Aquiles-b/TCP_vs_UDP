#include "../include/tcp_client.hpp"

using namespace tcp_vs_udp;

MessageType getMessageType(uint8_t *buffer) {
	return static_cast<MessageType>(buffer[0]);
}

TCPClient::TCPClient(char *addrstr, char *port) 
	: BasicClient(SOCK_STREAM, addrstr, port) {

}

int TCPClient::download(char *fname) {
	uint8_t txdatabuffer[1+2*sizeof(size_t)], *dlbuffer;
	ssize_t bytesread;
	size_t serverbuffersize, fsize, bsize;
	MessageType mtype;
	bool endtx = false;
	send(sock, fname, strnlen(fname, max_fname_size) + 1, 0);
	if ((bytesread = recv(sock, txdatabuffer, sizeof(txdatabuffer), 0)) < 0) {
		return 0;
	}
	else {
		mtype = getMessageType(txdatabuffer);
		std::cout << "bytes: " << bytesread << " type: " << mtype << "\n";
		switch (mtype) {
		case TXDATA:
			fsize = ((size_t *) (txdatabuffer+1))[0];
			(void) fsize; // Remove warning de variavel nao utilizada
			serverbuffersize = ((size_t *) (txdatabuffer+1))[1];
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
	FILE *f = fopen(fname, "w");
	if (!f) {
		std::cerr << "Error at opening file.\n";
		return 0;
	}
	bsize = serverbuffersize;
	dlbuffer = new uint8_t[bsize];
	while (((bytesread = recv(sock, dlbuffer, bsize, MSG_WAITALL)) > 0)) {
		std::cout << "bread: " << bytesread << " bsize: " << bsize << "\n";		
		fwrite(dlbuffer, 1, bytesread, f);
	}
	std::cout << "bread: " << bytesread << " bsize: " << bsize << "\n";
	if (bytesread < 0) {
		delete[] dlbuffer;
		fclose(f);
		return 0;
	}
	fwrite(dlbuffer, 1, bytesread, f);
	/*
	std::cout << "AAAAAAAAAAAAAAAAAA\n";
	while (!endtx && ((bytesread = recv(sock, dlbuffer, bsize, 0)) > 0)) {
		mtype = getMessageType(dlbuffer);
		std::cout << "bytesread: " << bytesread << " type: " << +mtype << "\n";
		switch (mtype) {
		case DATA:
			fwrite(dlbuffer+1, 1, bytesread-1, f);
			break;
		case ENDTX:
			endtx = true;
			break;
		default:
			std::cerr << "Unexpected message received. Terminating connection.\n" << "Type: " << mtype << "\n";
			delete[] dlbuffer;
			fclose(f);
			return 0;
			break;
		}
	}
	*/
	fclose(f);
	delete[] dlbuffer;
	return 1;
}
