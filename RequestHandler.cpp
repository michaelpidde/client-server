#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include "RequestHandler.h"

using namespace std;

namespace Network {
	RequestHandler::RequestHandler() {}

	RequestHandler::~RequestHandler() {}

	string RequestHandler::handle(string buffer) {
		requestContext rc = parseHeaders(buffer);

		// Hard coding this for now though it'll have to be calculated later.
		string response = "HTTP/1.1 200 OK";
		response += "\nContent-Type: text/html";
		response += "\nConnection: Closed";

		// Check for host configuration in server.
		// (Well, sooner or later. Hard coded for now.)
		struct knownHost {
			string host = "localhost:8888";
			string sysPath = "/var/www/testsite";
		};
		knownHost knownHost;

		string defaultFile = "index.htm";

		if(rc.host.compare(knownHost.host) == 0) {
			// Read from configured sysPath
			// (Some of this can probably be moved to a Directory class later.)
			struct stat check;
			if(stat(knownHost.sysPath.c_str(), &check) == 0 && S_ISDIR(check.st_mode)) {
				// If requested resource is a file, try to display it,
				// otherwise display directory listing.
				if(rc.request.resource.length() > 1) {
					//
				} else {
					// Try to display default file.
					std::ifstream file (knownHost.sysPath + "/" + defaultFile);
					std::stringstream fileBuffer;
					fileBuffer << file.rdbuf();
					response = fileBuffer.str();
				}
			}
		}

		return response;
	}

	request RequestHandler::parseRequest(string buffer) {
		request request;
		size_t start;

		// Get method. Only support GET and POST for the time being.
		size_t end = buffer.find(' ');
		string method = buffer.substr(0, end);
		if(strcmp(method.c_str(), "POST") == 0) {
			request.method = "POST";
		} else {
			request.method = "GET";
		}

		// Get resource (request path) (minus \n)
		start = buffer.find(' ', end);
		end = buffer.find(' ', start + 1);
		request.resource = buffer.substr(start + 1, end - start - 1);

		// Get http version (minus \n)
		start = buffer.find(' ', end);
		end = buffer.find(' ', start + 1);
		request.httpVersion = buffer.substr(start + 1, end - start - 1);

		return request;
	}

	requestContext RequestHandler::parseHeaders(string buffer) {
		requestContext rc;

		// First line is request itself (-1 to remove \n)
		size_t lineBreak = buffer.find('\n');
		string request = buffer.substr(0, lineBreak - 1);
		rc.request = parseRequest(request);

		// Second line is host (remove "Host: " and \n).
		size_t secondBreak = buffer.find('\n', lineBreak);
		rc.host = buffer.substr(lineBreak + 7, secondBreak - 1);

		// Start finding things...
		string needle = "Accept: ";
		size_t start = buffer.find(needle);
		size_t end;
		if(start != std::string::npos) {
			end = buffer.find('\n', start);
			rc.accept = buffer.substr(start + needle.length(), end - start);
		}

		needle = "Accept-Encoding: ";
		start = buffer.find(needle);
		if(start != std::string::npos) {
			end = buffer.find('\n', start);
			rc.encoding = buffer.substr(start + needle.length(), end - start);
		}

		needle = "User-Agent: ";
		start = buffer.find(needle);
		if(start != std::string::npos) {
			end = buffer.find('\n', start);
			rc.agent = buffer.substr(start + needle.length(), end - start);
		}

		needle = "Referer: ";
		start = buffer.find(needle);
		if(start != std::string::npos) {
			end = buffer.find('\n', start);
			rc.referer = buffer.substr(start + needle.length(), end - start);
		}

		return rc;
	}
}