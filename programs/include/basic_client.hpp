#ifndef _BASIC_CLIENT_H_
#define _BASIC_CLIENT_H_

#include <iostream>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "protocol.h"

namespace tcp_vs_udp {
	class BasicClient {
		protected:
			int sock;
			sockaddr_in saddr;
		public:
			BasicClient(int protocol, char *addrstr, char *port);
			~BasicClient();

			void connectToServer();

			virtual int download(char *fname);
		private:


	};
};

#endif