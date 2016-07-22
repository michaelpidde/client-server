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
			void socketConnect();
			void getHttpResponse(std::string url);
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