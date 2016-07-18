#include <iostream>
#include "SocketHandler.h"

using namespace std;
using namespace Network;

int main() {
	string url;
	cout << "URL: ";
	cin >> url;
	SocketHandler socketHandler;
	socketHandler.getAddressInfo(url);
	socketHandler.initSocket();
	socketHandler.socketConnect();
	socketHandler.getResponse(url);
	return 0;
}