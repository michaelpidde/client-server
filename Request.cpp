#include <iostream>
#include <regex>
#include <vector>
#include <sys/stat.h>
#include "File.h"
#include "Request.h"

using namespace std;

namespace Network {
	Request::Request() {}

	Request::~Request() {}

	response Request::handle(string buffer) {
		requestContext rc = parseHeaders(buffer);
		response response;

		// Hard coding this for now though it'll have to be calculated later.
		string staticHeader = "HTTP/1.1 200 OK";
		staticHeader += "\nContent-Type: text/html";
		staticHeader += "\nConnection: Closed";

		string binHeader = "HTTP/1.1 200 OK";

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
				// Check if a resource is requested. If not, try default file.
				// TODO: If neither of those work, display directory if configured to.
				string location = knownHost.sysPath + "/";
				bool returnBinary = false;
				string requestedFile;

				if(rc.request.resource.length() > 1) {
					requestedFile = rc.request.resource;
				} else {
					requestedFile = defaultFile;
				}

				// Get file extension and check against valid binary types
				regex regex("[a-zA-Z]+$");
				smatch match;
				if(regex_search(rc.request.resource, match, regex)) {
					if(find(begin(binTypes), end(binTypes), match[0]) != end(binTypes)) {
						returnBinary = true;
						binHeader += "\nContent-Type: image/" + (string)match[0];
					}
				}

				location += requestedFile;

				vector<char> contents;
				try {
					contents = File::readStatic(location, returnBinary);
				} catch(const char* e) {
					// TODO: Log this error.
					error404(response, requestedFile);
					return response;
				}

				if(returnBinary) {
					binHeader += "\nContent-Transfer-Encoding: binary";
					binHeader += "\nContent-Length: " + to_string(contents.size());
					binHeader += "\nConnection: Closed";
					response.headers = binHeader + "\n\n";
					response.binary = true;
				} else {
					response.headers = staticHeader + "\n\n";
					response.binary = false;
				}

				response.body = contents;
			} else {
				// Configured host directory doesn't exist.
				// TODO: Log this error.
				error500(response);
				return response;
			}
		} else {
			// No known host defined.
			// TODO: Log this error.
			error500(response);
			return response;
		}

		return response;
	}

	void Request::error404(response &response, string &requestedFile) {
		string errorHeaders = "HTTP/1.1 404 Not Found";
		errorHeaders += "\nContent-Type: text/html";
		response.binary = false;
		string body = htmlTemplate(
			"404 Not Found",
			"Could not find requested file <b>" + requestedFile + "</b> on this server."
		);
		errorHeaders += "\nContent-Length: " + to_string(body.length());

		response.headers = errorHeaders + "\n\n";
		response.body = vector<char>(body.begin(), body.end());
	}

	void Request::error500(response &response) {
		string errorHeaders = "HTTP/1.1 500";
		errorHeaders += "\nContent-Type: text/html";
		response.binary = false;
		string body = htmlTemplate("Internal Server Error", "Internal Server Error");
		errorHeaders += "\nContent-Length: " + to_string(body.length());

		response.headers = errorHeaders + "\n\n";
		response.body = vector<char>(body.begin(), body.end());
	}

	string Request::htmlTemplate(string title, string body) {
		string html = "<doctype html>";
		html += "\n<html>";
		html += "\n<head><title>" + title + "</title></head>";
		html += "\n<body>" + body + "</body>";
		html += "\n</html>";
		return html;
	}

	request Request::parseRequest(string buffer) {
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

	requestContext Request::parseHeaders(string buffer) {
		requestContext rc;

		// First line is request itself (-1 to remove \n)
		size_t lineBreak = buffer.find('\n');
		string request = buffer.substr(0, lineBreak - 1);
		rc.request = parseRequest(request);

		// Start grabbing particular headers.
		regex regex("(Host: ([a-zA-Z0-9.:-]+))");
		smatch match;
		if(regex_search(buffer, match, regex)) {
			rc.host = match[2];
		}

		regex = "(Accept: ([a-zA-Z0-9.:\\/,\\+;=\\* -_]+))";
		if(regex_search(buffer, match, regex)) {
			rc.accept = match[2];
		}

		regex = "(Accept-Encoding: ([a-zA-Z0-9.:\\/,\\+;=\\* -_]+))";
		if(regex_search(buffer, match, regex)) {
			rc.encoding = match[2];
		}

		regex = "(User-Agent: ([\\(a-zA-Z0-9.:\\/,\\+;=\\* -_\\)]+))";
		if(regex_search(buffer, match, regex)) {
			rc.agent = match[2];
		}

		regex = "(Referer: ([a-zA-Z0-9.:\\/,\\+= -_]+))";
		if(regex_search(buffer, match, regex)) {
			rc.referer = match[2];
		}

		return rc;
	}
}