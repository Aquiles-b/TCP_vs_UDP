#include "../include/udp_server.hpp"
#include <cstdio>
#include <sys/socket.h>

using namespace tcp_vs_udp;

UDPServer::UDPServer(const std::string& ip_address, const int& port_number) :
    BasicServer(ip_address, port_number, SOCK_STREAM) {
}

void UDPServer::handleClient(int clientfd, sockaddr_in clientaddr) {
	char fname[256];
	recv(clientfd, fname, sizeof(fname), 0);
	FILE *fp = fopen(fname, "r");
	if (fp == NULL) {
		sendError(clientfd, clientaddr);
	}
	else {
		sendFile(clientfd, clientaddr, fp);
		fclose(fp);
	}

	close(clientfd);
}

void UDPServer::sendFile(int clientfd, sockaddr_in &caddr, FILE *file) {
    u_int8_t *sliding_window, *buffer_aux, client_buf[2], client_seq, current_seq=0;
    size_t tam_last_message = 0;
    try {
        sliding_window = new u_int8_t[this->win_size*(this->buffersize+2)*2];
    }
    catch (...) {
        std::cerr << "Error: Unable to create sliding window. " << errno << std::endl;
        return;
    }
    sockaddr *cadd_aux = (sockaddr *) &caddr;
    socklen_t cadd_len = sizeof(caddr);
    size_t &bsize = this->buffersize;

    int win_init_idx = 0, num_new_messages=0;

    num_new_messages = fill_sliding_window(sliding_window, file, win_init_idx,
                                            num_new_messages, current_seq, tam_last_message);

    while (1) {
        // Envia janela
        if (!send_window(clientfd, sliding_window, caddr, win_init_idx, tam_last_message)) {
            std::cerr << "Error: send_window: " << errno << std::endl;
            break;
        }

        // Preencher buffer caso esteja perto da metade
        if (!tam_last_message && (num_new_messages - this->win_size) <= this->win_size) {
            num_new_messages = fill_sliding_window(sliding_window, file, win_init_idx,
                                                    num_new_messages, current_seq, tam_last_message);
        }

        // Recebe resposta do client
        if (recvfrom(clientfd, client_buf, 2, 0, cadd_aux, &cadd_len) < 0) {
            std::cerr << "Error: recvfrom: " << errno << std::endl;
            break;
        }
        
        if (client_buf[0] == MessageType::ACK) {
            win_init_idx = (win_init_idx + this->win_size) % (this->win_size*2);
            num_new_messages -= this->win_size;
        }
        else {
            buffer_aux = sliding_window + win_init_idx*bsize;
            client_seq = client_buf[1];

            win_init_idx = (win_init_idx + (client_seq - buffer_aux[1])) % (this->win_size*2);
            num_new_messages -= client_seq - buffer_aux[1];
        }
    }
    delete[] sliding_window;
}

bool UDPServer::send_window(const int&clientfd, u_int8_t *sliding_window,
                            const sockaddr_in &caddr, const int &win_init_idx, const size_t &tam_last_message) {
    socklen_t caddr_len = sizeof(caddr);
    sockaddr *caddr_aux = (sockaddr *) &caddr;
    size_t &bsize = this->buffersize;
    size_t num_buffers = this->win_size*2;
    int current_msg_idx = 0;
    u_int8_t *buffer_aux;

    for (int i = 0; i < this->win_size; i++) {
        current_msg_idx = (win_init_idx+i) % num_buffers;
        buffer_aux = sliding_window + bsize*current_msg_idx;

        if (buffer_aux[0] == MessageType::DATA) {
            if (sendto(clientfd, buffer_aux, bsize, 0, caddr_aux, caddr_len) < 0) {
                return false;
            }
        }
        else {
            if (sendto(clientfd, buffer_aux, tam_last_message, 0, caddr_aux, caddr_len) < 0) {
                return false;
            }
            return true;
        }
    }
    return true;
}

int UDPServer::fill_sliding_window(u_int8_t *sliding_window, FILE *file, const int &win_init_idx, 
                                    const int &num_new_messages, u_int8_t &current_seq, size_t &tam_last_message) {
    size_t &bsize = this->buffersize, bytes_read;
    int i = (win_init_idx + num_new_messages) % (this->win_size*2), count = 0;
    for (; i < this->win_size*2 - num_new_messages; i++) {
        bytes_read = fread(sliding_window + 2 + i*bsize, 1, bsize, file);
        if (bytes_read > 0) {
            sliding_window[i*bsize] = MessageType::DATA;
            sliding_window[i*bsize + 1] = current_seq;
            current_seq++;
        }
        else {
            i--;
            sliding_window[i*buffersize] = MessageType::ENDTX;
            tam_last_message = bytes_read;
            break;
        }
        count++;
    }
    return count + num_new_messages;
}

void UDPServer::setWinSize(size_t win_size) {
    this->win_size = win_size;
}
