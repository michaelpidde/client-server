#include <iostream>
#include "SocketHandler.h"

using namespace std;
using namespace Network;

int main() {
	SocketHandler socketHandler;
	socketHandler.getAddressInfo();
	socketHandler.initSocket();
	socketHandler.socketConnect();
	socketHandler.getResponse();
	return 0;
}