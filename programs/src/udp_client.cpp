#include "../include/udp_client.hpp"
#include <cstdio>
#include <sys/socket.h>

using namespace tcp_vs_udp;

UDPClient::UDPClient(char *addrstr, char *port) 
	: BasicClient(SOCK_STREAM, addrstr, port), win_size{5}, init_win_idx{0}, bsize{4096}, 
    last_message_size{0}, current_seq{0} {

    struct timeval timeout;
    // Configurando o timeout para recvfrom()
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;  // 100.000 microssegundos = 100 milissegundos;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("Error: Unable to set timeout");
    }
}

int UDPClient::download(char *fname) {
    std::string filename = std::string(fname);

    filename = "programs/downloads/" + filename;

	FILE *f = fopen(filename.c_str(), "w");
	if (!f) {
		std::cerr << "Error at opening file.\n";
		return 0;
	}
    if (!get_buffer_and_win_size(fname)) {
        std::cerr << "Error: get_buffer_and_win_size: " << errno << std::endl;
        return 0;
    }
    this->init_sliding_win_buf();

    u_int8_t *sliding_win[this->win_size];
    int num_valid_messages;
    bool is_finished = false;

    while (!is_finished) {
        if ((num_valid_messages = recv_window(sliding_win)) == -1) {
            std::cerr << "Error: recv_window: " << errno << std::endl;
            return 0;
        }

        for (int i = 0; i < num_valid_messages; i++) {
            if (getMessageType(sliding_win[i]) != MessageType::ENDTX) {
                fwrite(sliding_win[i] + 2, 1, bsize-2, f);
            }
            else {
                fwrite(sliding_win[i] + 2, 1, last_message_size-2, f);
                is_finished = true;
            }
        }
        this->init_win_idx = (init_win_idx + num_valid_messages) % win_size;

        if ((num_valid_messages == this->win_size) || is_finished) {
            send_ACK();
        }
        else {
            send_NACK();
        }
    }

    delete[] this->sliding_win_buf;
	return 1;
}

int UDPClient::recv_window(u_int8_t **sliding_win) {
    sockaddr *saddr_aux = (sockaddr*) &saddr;
    socklen_t saddr_len = sizeof(saddr);
    u_int8_t *buf_aux, mtype;
    int num_bytes_read;

    int current_idx;
    int message_ordered_idx;

    int messages_recv = 0;

    for (int i = 0; i < this->win_size; i++) {
        current_idx = (i + init_win_idx) % this->win_size;
        buf_aux = this->sliding_win_buf + current_idx*bsize;

        if ((num_bytes_read = recvfrom(sock, buf_aux, bsize, 0, saddr_aux, &saddr_len)) < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            } 
            else {
                return -1;
            }
        }
        mtype = getMessageType(buf_aux);
        if (mtype != MessageType::DATA && mtype != MessageType::ENDTX) {
            continue;
        }
        message_ordered_idx = buf_aux[1] - current_seq;
        if (message_ordered_idx < win_size && message_ordered_idx >= 0) {
            sliding_win[message_ordered_idx] = buf_aux;
            messages_recv++;
        }
        else {
            continue;
        }
        if (mtype == MessageType::ENDTX) {
            break;
        }
    }

    int j = 0;
    for (j = 0; j < messages_recv; j++) {
        if (sliding_win[j][1] != current_seq) {
            break;
        }
        if (getMessageType(buf_aux) == MessageType::ENDTX) {
            this->last_message_size = num_bytes_read;
            j++;
            break;
        }
        current_seq++;
    }
    return j;
}

bool UDPClient::get_buffer_and_win_size(char *fname) {
	uint8_t txdatabuffer[1+3*sizeof(size_t)];
	size_t fsize;
	MessageType mtype;

	if (sendto(sock, fname, strnlen(fname, max_fname_size) + 1, 0, 
                                    (sockaddr *) &saddr, sizeof(saddr)) < 0) {
        std::cerr << "Error: sendto: " << errno << std::endl;
    }
    socklen_t saddr_len = sizeof(saddr);
	if (recvfrom(sock, txdatabuffer, sizeof(txdatabuffer), 0,
                                        (sockaddr *) &saddr, &saddr_len) < 0) {
		return false;
	}
	else {
		mtype = getMessageType(txdatabuffer);
		switch (mtype) {
		case TXDATA:
			fsize = ((size_t *) (txdatabuffer+1))[0];
			this->bsize = ((size_t *) (txdatabuffer+1))[1];
			this->win_size = static_cast<int>(((size_t *) (txdatabuffer+1))[2]);
			(void) fsize; // Remove warning de variavel nao utilizada
            send_ACK();
            return true;
			break;
		case ERROR:
			std::cerr << "Server error ocurred.\n";
			return false;
			break;
		default:
			std::cerr << "Unexpected message received. Terminating connection.\n";
			return false;
			break;
		}
	}
}

void UDPClient::init_sliding_win_buf() {
    try {
        this->sliding_win_buf = new u_int8_t[this->win_size*bsize];
    }
    catch (std::bad_alloc &ba) {
        std::cerr << "Error: " << ba.what() << std::endl;
        exit(1);
    }
    last_message_size = 0;
    init_win_idx = 0;
    current_seq = 0;
}

void UDPClient::setWinSize(int win_size) {
    this->win_size = win_size;
}

void UDPClient::send_ACK() const {
    u_int8_t buf[2];
    buf[0] = MessageType::ACK;
    sendto(sock, buf, 2, 0, (sockaddr *) &saddr, sizeof(saddr));
}

void UDPClient::send_NACK() const {
    u_int8_t buf[2];
    buf[0] = MessageType::NACK;
    buf[1] = this->current_seq;
    sendto(sock, buf, 2, 0, (sockaddr *) &saddr, sizeof(saddr));
}