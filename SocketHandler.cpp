#include <iostream>
#include <errno.h>
#include <string>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <netdb.h>
#include "SocketHandler.h"

using namespace std;

namespace Network {
	SocketHandler::SocketHandler() {
		socketId = 0;
	}

	SocketHandler::SocketHandler(bool verbose, int port) {
		socketId = 0;
		this->verbose = verbose;
		this->port = port;
		cout << "Server running on port " << to_string(port) << endl;
		cout << "Press Ctrl + C to quit" << endl;
	}

	SocketHandler::~SocketHandler() {
		freeaddrinfo(hostList);
		close(socketId);
	}

	void SocketHandler::getAddressInfo(string url) {
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

	void SocketHandler::getAddressInfo(const char* port) {
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

	void SocketHandler::initSocket() {
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

	void SocketHandler::bindSocket() {
		if(verbose) {
			cout << "Binding socket..." << endl;
		}
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

	int SocketHandler::handleProcess(int newSocketId) {
		if(verbose) {
			cout << "Receiving request data..." << endl;
		}
		char buffer[2000];
		// Clean out the memory.
		memset(&buffer, 0, 2000);
		recv(newSocketId, &buffer, 2000, 0);
		if(verbose) {
			cout << buffer << endl;
		}

		// Split headers at new line.
		vector<string> headers;
		string item;
		stringstream ssin(buffer);
		while(getline(ssin, item, '\n')) {
			headers.push_back(item);
		}


		// if(verbose) {
		// 	cout << "Sending back default page..." << endl;
		// }
		// string response = "HTTP/1.1 200 OK";
		// response += "\nContent-Type: text/html";
		// response += "\nConnection: Closed";
		// response += "\n\n";
		// response += "<!doctype html>\n";
		// response += "<html>\n<head>\n<title></title>\n</head>\n<body>\n";
		// response += "Server working.\n</body>\n</html>";

		// int len = strlen(response.c_str());
		// ssize_t sentBytes = send(newSocketId, response.c_str(), len, 0);
		return 0;
	}


	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	 *
	 * CLIENT FUNCTIONS
	 *
	 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	 */
	void SocketHandler::socketConnect() {
		if(verbose) {
			cout << "Connecting to socket..." << endl;
		}
		int status = connect(socketId, hostList->ai_addr, hostList->ai_addrlen);
		if(status == -1) {
			cerr << "Error connecting to socket." << endl;
		}
	}

	void SocketHandler::getHttpResponse(string url) {
		if(verbose) {
			cout << "Sending..." << endl;
		}
		string msg = "GET / HTTP/1.1\nhost: " + url + "\n\n";
		int len = strlen(msg.c_str());
		ssize_t bytesSent = send(socketId, msg.c_str(), len, 0);

		if(verbose) {
			cout << "Receiving message..." << endl;
		}
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