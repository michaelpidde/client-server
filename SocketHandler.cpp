#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "SocketHandler.h"

using namespace std;

namespace Network {
	SocketHandler::SocketHandler() {
		socketId = 0;
	}

	SocketHandler::~SocketHandler() {
		freeaddrinfo(hostList);
	}

	int SocketHandler::getSocketId() {
		return socketId;
	}

	void SocketHandler::getAddressInfo(string url) {
		int status;
		struct addrinfo hostData = {};

		hostData.ai_family = AF_UNSPEC;
		hostData.ai_socktype = SOCK_STREAM;

		status = getaddrinfo(url.c_str(), "80", &hostData, &hostList);

		if(status != 0) {
			cerr << "Error getting host info: " << gai_strerror(status) << endl;
		} else {
			cout << "Got address info. " << endl;
		}
	}

	void SocketHandler::initSocket() {
		cout << "Creating socket..." << endl;
		int id = socket(hostList->ai_family, hostList->ai_socktype, hostList->ai_protocol);
		if(id == -1) {
			cerr << "Error getting socket." << endl;
		} else {
			socketId = id;
		}
	}

	void SocketHandler::socketConnect() {
		cout << "Connecting to socket..." << endl;
		int status = connect(socketId, hostList->ai_addr, hostList->ai_addrlen);
		if(status == -1) {
			cerr << "Error connecting to socket." << endl;
		}
	}

	void SocketHandler::getResponse(string url) {
		cout << "Sending..." << endl;
		string msg = "GET / HTTP/1.1\nhost: " + url + "\n\n";
		int len = strlen(msg.c_str());
		ssize_t bytesSent = send(socketId, msg.c_str(), len, 0);

		cout << "Receiving message..." << endl;
		char incomingBuffer[1000];
		ssize_t bytesReceived = recv(socketId, incomingBuffer, 1000, 0);
		if(bytesReceived == 0) {
			cout << "Host shut connection." << endl;
		} else if(bytesReceived == -1) {
			cerr << "Error receiving response." << endl;
		}

		cout << bytesReceived << " bytes received." << endl;
		cout << incomingBuffer << endl;
	}
}