#include <iostream>
#include <errno.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include "SocketHandler.h"

using namespace std;

namespace Network {
	SocketHandler::SocketHandler() {
		socketId = 0;
	}

	SocketHandler::~SocketHandler() {
		freeaddrinfo(hostList);
		close(socketId);
	}

	int SocketHandler::getSocketId() {
		return socketId;
	}

	void SocketHandler::getAddressInfo(string url) {
		struct addrinfo hostData = {};

		hostData.ai_family = AF_UNSPEC;
		hostData.ai_socktype = SOCK_STREAM;

		int status = getaddrinfo(url.c_str(), "80", &hostData, &hostList);

		if(status != 0) {
			cerr << "Error getting host info: " << gai_strerror(status) << endl;
		} else {
			cout << "Got address info. " << endl;
		}
	}

	void SocketHandler::getAddressInfo(const char* port) {
		struct addrinfo hostData;
		memset(&hostData, 0, sizeof(struct addrinfo));

		hostData.ai_family = AF_UNSPEC;
		hostData.ai_flags = AI_PASSIVE;
		hostData.ai_socktype = SOCK_STREAM;

		int status = getaddrinfo(NULL, port, &hostData, &hostList);

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

	void SocketHandler::bindSocket() {
		cout << "Binding socket..." << endl;
		// Check if port is in use first.
		int reuse = 1;
		int status = setsockopt(socketId, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
		status = bind(socketId, hostList->ai_addr, hostList->ai_addrlen);

		if(status == -1) {
			char buffer[256];
			int errorMessage = strerror_r(errno, buffer, 256);
			cerr << "Error binding socket: " << buffer << endl;
		}
	}

	void SocketHandler::socketListen(int backlog) {
		cout << "Listening..." << endl;
		int status = listen(socketId, backlog);
		if(status == -1) {
			cerr << "Error listening." << endl;
		}

		int newSocketId;
		struct sockaddr_storage address;
		socklen_t addressSize = sizeof(address);
		newSocketId = accept(socketId, (struct sockaddr *)&address, &addressSize);
		if(newSocketId == -1) {
			cerr << "Error accepting." << endl;
		} else {
			socketId = newSocketId;
		}

		write();
	}

	int SocketHandler::write() {
		// Do stuff here with connection...
		cout << "Sending back default page..." << endl;
		string response = "HTTP/1.1 200 OK";
		response += "\nContent-Type: text/html";
		response += "\nConnection: Closed";
		response += "\n\n";
		response += "<!doctype html>";
		response += "<html><head><title></title></head><body>Server working.</body></html>";

		int len = strlen(response.c_str());
		ssize_t sentBytes = send(socketId, response.c_str(), len, 0);
		return 0;
	}


	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	 *
	 * CLIENT FUNCTIONS
	 *
	 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	 */
	void SocketHandler::socketConnect() {
		cout << "Connecting to socket..." << endl;
		int status = connect(socketId, hostList->ai_addr, hostList->ai_addrlen);
		if(status == -1) {
			cerr << "Error connecting to socket." << endl;
		}
	}

	void SocketHandler::getHttpResponse(string url) {
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