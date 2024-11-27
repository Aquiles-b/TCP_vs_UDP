#include "../include/udp_server.hpp"

using namespace tcp_vs_udp;

UDPServer::UDPServer(const std::string& ip_address, const int& port_number) :
    BasicServer(ip_address, port_number, SOCK_STREAM), win_size{5} {
    std::cout << "Socket opened." << std::endl;
}

UDPServer::UDPServer(const std::string& ip_address, const int& port_number, const int& win_size) :
    UDPServer(ip_address, port_number) {
    this->win_size = win_size;
}

void UDPServer::handleClient(int clientfd, sockaddr_in clientaddr) {
	char fname[256];
    struct timeval timeout;
    // Configurando o timeout para recvfrom()
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;  // 100.000 microssegundos = 100 milissegundos;
    if (setsockopt(clientfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("Error: Unable to set timeout");
    }

    socklen_t caddr_len = sizeof(clientaddr);
	if (recvfrom(clientfd, fname, sizeof(fname), 0, (sockaddr *) &clientaddr, &caddr_len) < 0) {
        std::cerr << "Error: recvfrom: " << errno << std::endl;
        sendError(clientfd, clientaddr);
        return;
    }

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
    if (!send_file_and_buffer_info(clientfd, caddr, file, this->buffersize)) {
        std::cerr << "Error: send_file_and_buffer_info: " << errno << std::endl;
        return;
    }

    uint8_t *sliding_window, *buffer_aux, client_buf[2], client_seq, current_seq=0;
    size_t last_message_size = 0;
    try {
        sliding_window = new uint8_t[this->win_size*this->buffersize*2];
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
                                            num_new_messages, current_seq, last_message_size);
    while (1) {
        // Envia janela
        if (!send_window(clientfd, sliding_window, caddr, win_init_idx, last_message_size)) {
            std::cerr << "Error: send_window: " << errno << std::endl;
            break;
        }

        // Preencher buffer caso esteja perto da metade
        if (!last_message_size && (num_new_messages - this->win_size) < this->win_size) {
            num_new_messages = fill_sliding_window(sliding_window, file, win_init_idx,
                                                    num_new_messages, current_seq, last_message_size);
        }

        // Recebe resposta do client
        if (recvfrom(clientfd, client_buf, 2, 0, cadd_aux, &cadd_len) < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            } 
            else {
                std::cerr << "Error: recvfrom: " << errno << std::endl;
                break;
            }
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

        if (num_new_messages <= 0) {
            break;
        }
    }
    delete[] sliding_window;
}

bool UDPServer::send_window(const int&clientfd, uint8_t *sliding_window,
                            const sockaddr_in &caddr, const int &win_init_idx, const size_t &last_message_size) {
    socklen_t caddr_len = sizeof(caddr);
    sockaddr *caddr_aux = (sockaddr *) &caddr;
    size_t &bsize = this->buffersize;
    size_t num_buffers = this->win_size*2;
    int current_msg_idx = 0;
    uint8_t *buffer_aux;

    for (int i = 0; i < this->win_size; i++) {
        current_msg_idx = (win_init_idx+i) % num_buffers;
        buffer_aux = sliding_window + bsize*current_msg_idx;
        if (buffer_aux[0] == MessageType::DATA) {
            if (sendto(clientfd, buffer_aux, bsize, 0, caddr_aux, caddr_len) < 0) {
                return false;
            }
        }
        else {
            // +2 para enviar o byte do tipo e o byte do seq
            if (sendto(clientfd, buffer_aux, last_message_size+2, 0, caddr_aux, caddr_len) < 0) {
                return false;
            }
            return true;
        }
    }
    return true;
}

int UDPServer::fill_sliding_window(uint8_t *sliding_window, FILE *file, const int &win_init_idx, 
                                    const int &num_new_messages, uint8_t &current_seq, size_t &last_message_size) {
    size_t &bsize = this->buffersize, bytes_read, last_bytes_read;
    int cur_win_idx = (win_init_idx + num_new_messages) % (this->win_size*2), count = 0;
    int cur_idx;
    for (int i = 0; i < this->win_size*2 - num_new_messages; i++) {
        cur_idx = (cur_win_idx + i) % (this->win_size*2);
        bytes_read = fread(sliding_window + 2 + cur_idx*bsize, 1, bsize-2, file);
        if (bytes_read > 0) {
            (sliding_window + cur_idx*bsize)[0] = MessageType::DATA;
            (sliding_window + cur_idx*bsize)[1] = current_seq;
            current_seq++;
        }
        else {
            i--;
            cur_idx = (cur_win_idx + i) % (this->win_size*2);
            (sliding_window + cur_idx*bsize)[0] = MessageType::ENDTX;
            last_message_size = last_bytes_read;
            break;
        }
        count++;
        last_bytes_read = bytes_read;
    }
    return count + num_new_messages;
}

bool UDPServer::send_file_and_buffer_info(int clientfd, sockaddr_in &caddr, FILE *file,
                                                                    const size_t &bsize) {
	uint8_t buffer[1+3*sizeof(size_t)], client_buffer[2];
	buffer[0] = MessageType::TXDATA;
	fseek(file, 0, SEEK_END); 
	size_t fsize = ftell(file); // Pega o tamanho
	((size_t*) (buffer+1))[0] = fsize; // Coloca tamanho na mensagem
	((size_t*) (buffer+1))[1] = bsize; // Coloca tamanho do buffer na mensagem
    ((size_t*) (buffer+1))[2] = static_cast<size_t>(this->win_size); // Coloca tamanho da janela na mensagem
	// Mensagem:
	// MTYPE (uint8) | FILE SIZE (size_t) | SERVER TRANSMISSION BUFFER SIZE (size_t) | WINDOW SIZE (size_t)
    socklen_t caddr_len = sizeof(caddr);
    sockaddr *caddr_aux = (sockaddr *) &caddr;
    while (1) {
        if (sendto(clientfd, buffer, 1+3*sizeof(size_t), 0, caddr_aux, sizeof(caddr)) < 0){
            return false;
        }
        if (recvfrom(clientfd, client_buffer, 2, 0, caddr_aux, &caddr_len) < 0) {
            return false;
        }
        if (client_buffer[0] == MessageType::ACK) {
            break;
        }
    }

    fseek(file, 0, SEEK_SET);

    return true;
}

void UDPServer::setWinSize(size_t win_size) {
    this->win_size = win_size;
}
