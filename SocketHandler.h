#include <netdb.h>

namespace Network {
	class SocketHandler {
		public:
			SocketHandler();
			SocketHandler(bool verbose, int port);
			~SocketHandler();
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