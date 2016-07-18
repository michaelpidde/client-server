#include <iostream>
#include <netdb.h>

namespace Network {
	class SocketHandler {
		public:
			SocketHandler();
			~SocketHandler();
			void getAddressInfo(std::string url);
			void initSocket();
			int getSocketId();
			void socketConnect();
			void getResponse(std::string url);
		private:
			int socketId;
			addrinfo *hostList;
	};
}