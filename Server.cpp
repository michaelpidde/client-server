#include <iostream>
#include "SocketHandler.h"

using namespace std;
using namespace Network;

int main(int argc, char *argv[]) {
	int port = 9090;

	// Check for p flag and see if a valid port number is supplied.
	if(argc == 3) {
		if(strcmp(argv[1], "-p") == 0) {
			int input = atoi(argv[2]);
			if(input <= 1024 || input > 65535) {
				cout << "Specified port must be > 1024 and <= 65535." << endl;
				cout << "Using port 9090." << endl;
			} else {
				port = input;
			}
		}
	}

	SocketHandler socketHandler;
	socketHandler.getAddressInfo(to_string(port).c_str());
	socketHandler.initSocket();
	socketHandler.bindSocket();
	socketHandler.socketListen(5);
}