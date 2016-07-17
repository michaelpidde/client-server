#include <iostream>
#include <netdb.h>

namespace Network {
	class SocketHandler {
		public:
			SocketHandler();
			~SocketHandler();
			void getAddressInfo();
			void initSocket();
			int getSocketId();
			void socketConnect();
			void getResponse();
		private:
			int socketId;
			addrinfo *hostList;
	};
}