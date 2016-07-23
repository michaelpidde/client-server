#include <iostream>
#include <errno.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include "Socket.h"
#include "Request.h"

using namespace std;

namespace Network {
	Socket::Socket() {
		socketId = 0;
	}

	Socket::Socket(bool verbose, int port) {
		socketId = 0;
		this->verbose = verbose;
		this->port = port;
		cout << "Server running on port " << to_string(port) << endl;
		cout << "Press Ctrl + C to quit" << endl;
	}

	Socket::~Socket() {
		freeaddrinfo(hostList);
		close(socketId);
	}

	void Socket::getAddressInfo(string url) {
		if(verbose) {
			cout << "Getting address info. " << endl;
		}

		struct addrinfo hostData;
		memset(&hostData, 0, sizeof(struct addrinfo));

		hostData.ai_family = AF_UNSPEC;
		hostData.ai_socktype = SOCK_STREAM;

		int status = getaddrinfo(url.c_str(), "80", &hostData, &hostList);

		if(status != 0) {
			cerr << "Error getting host info: " << gai_strerror(status) << endl;
		}
	}

	void Socket::getAddressInfo(const char* port) {
		if(verbose) {
			cout << "Getting address info. " << endl;
		}

		struct addrinfo hostData;
		memset(&hostData, 0, sizeof(struct addrinfo));

		hostData.ai_family = AF_UNSPEC;
		hostData.ai_flags = AI_PASSIVE;
		hostData.ai_socktype = SOCK_STREAM;

		int status = getaddrinfo(NULL, port, &hostData, &hostList);

		if(status != 0) {
			cerr << "Error getting host info: " << gai_strerror(status) << endl;
		}
	}

	void Socket::initSocket() {
		if(verbose) {
			cout << "Creating socket..." << endl;
		}
		int id = socket(hostList->ai_family, hostList->ai_socktype, hostList->ai_protocol);
		if(id == -1) {
			cerr << "Error getting socket." << endl;
		} else {
			socketId = id;
		}
	}

	void Socket::bindSocket() {
		if(verbose) {
			cout << "Binding socket..." << endl;
		}
		// Check if port is in use first.
		int reuse = 1;
		int status = setsockopt(socketId, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
		status = ::bind(socketId, hostList->ai_addr, hostList->ai_addrlen);

		if(status == -1) {
			char buffer[256];
			int errorMessage = strerror_r(errno, buffer, 256);
			cerr << "Error binding socket: " << buffer << endl;
		}
	}

	void Socket::socketListen(int backlog) {
		if(verbose) {
			cout << "Listening..." << endl;
		}
		int status = listen(socketId, backlog);
		if(status == -1) {
			cerr << "Error listening." << endl;
		}

		while(true) {
			struct sockaddr_storage address;
			socklen_t addressSize = sizeof(address);
			int newSocketId = accept(socketId, (struct sockaddr *)&address, &addressSize);
			if(newSocketId == -1) {
				cerr << "Error accepting." << endl;
			}

			pid_t pid = fork();
			if(pid == -1) {
				cerr << "Error forking child process." << endl;
			} else if(pid == 0) {
				close(socketId);
				handleProcess(newSocketId);
				close(newSocketId);
				_exit(0);
			} else {
				close(newSocketId);
			}
		}
	}

	int Socket::handleProcess(int newSocketId) {
		if(verbose) {
			cout << "Receiving request data..." << endl;
		}
		// TODO: Clean this up since it's conceivable that a request could be
		// larger than this character allotment.
		char buffer[2000];
		// Clean out the memory.
		memset(&buffer, 0, 2000);
		recv(newSocketId, &buffer, 2000, 0);
		if(verbose) {
			cout << buffer << endl;
		}

		Request rh;
		Request::response response = rh.handle((string)buffer);

		int len = strlen(response.headers.c_str());
		ssize_t sentBytes = send(newSocketId, response.headers.c_str(), len, 0);

		if(response.binary) {
			len = response.body.size();
			sentBytes = send(newSocketId, response.body.data(), len, 0);
		} else {
			string body(response.body.begin(), response.body.end());
			len = body.length();
			sentBytes = send(newSocketId, body.c_str(), len, 0);
		}

		return 0;
	}
}