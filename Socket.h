#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <netdb.h>

namespace Network {
	class Socket {
		public:
			Socket();
			Socket(bool verbose, int port);
			~Socket();
			void getAddressInfo(const char* port);
			void getAddressInfo(std::string url);
			void initSocket();
			void bindSocket();
			void socketListen(int backlog);
			int handleProcess(int newSocketId);
			int getSocketId();
		private:
			int socketId;
			bool verbose;
			int port;
			addrinfo *hostList;
	};
}

#endif /*_SOCKET_H_*/