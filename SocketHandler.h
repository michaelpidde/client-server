#include <iostream>
#include <netdb.h>

namespace Network {
	class SocketHandler {
		public:
			SocketHandler();
			~SocketHandler();
			void getAddressInfo(const char* port);
			void getAddressInfo(std::string url);
			void initSocket();
			void bindSocket();
			void socketConnect();
			void getHttpResponse(std::string url);
			void socketListen(int backlog);
			int write();
			int getSocketId();
		private:
			int socketId;
			addrinfo *hostList;
	};
}