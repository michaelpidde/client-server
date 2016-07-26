#include <iostream>
#include <string>
#include <vector>
#include <syslog.h>
#include "Config.h"
#include "File.h"
#include "Request.h"

using namespace std;

namespace Network {
	Request::Request() {}

	Request::~Request() {}

	Request::response Request::handle(string buffer) {
		response response;
		knownHost knownHost;
		requestContext rc;

		try {
			rc = parseHeaders(buffer);
		} catch(const char* e) {
			// Cast to string to use more easily.
			string method(e);
			logError("Unimplemented HTTP method '" + method + "'");
			status405(response, method);
			return response;
		}

		try {
			knownHost = getKnownHost(rc.host);
		} catch(const char* e) {
			logError("Could not find host '" + rc.host + "' in configured hosts.");
			status500(response);
			return response;
		}

		// TODO: Make this configurable.
		string defaultFile = "index.htm";

		// Check configured host directory
		if(!File::isValidDirectory(knownHost.sysPath)) {
			logError("Configured host directory '" + knownHost.sysPath + "' is not valid.");
			status500(response);
			return response;
		}

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
		string ext = File::getFileExtension(requestedFile);
		if(find(begin(binTypes), end(binTypes), ext) != end(binTypes)) {
			returnBinary = true;
		}

		location += requestedFile;

		// Read requested file contents from file system.
		vector<char> contents;
		try {
			contents = File::readStatic(location, returnBinary);
		} catch(const char* e) {
			// TODO: Log this error.
			status404(response, requestedFile);
			return response;
		}
		response.body = contents;

		// If we got here, everything's good, so send a 200.
		status200(response, returnBinary, ext);
		return response;
	}

	knownHost Request::getKnownHost(string host) {
		// Check for host configuration in server.
		// (Well, sooner or later. Hard coded for now.)
		knownHost knownHost;
		knownHost.host = "localhost:8888";
		knownHost.sysPath = "/var/www/testsite";

		if(host.compare(knownHost.host) == 0) {
			return knownHost;
		} else {
			throw "500";
		}
	}

	void Request::status200(response &response, bool binary, string ext) {
		string headers = "HTTP/1.1 200 OK";

		if(binary) {
			response.binary = true;
			headers += "\nContent-Type: image/" + ext;
			headers += "\nContent-Transfer-Encoding: binary";
		} else {
			response.binary = false;
			if(ext.length()) {
				headers += "\nContent-Type: text/" + (ext == "htm"? "html" : ext);
			}
		}

		headers += "\nContent-Length: " + to_string(response.body.size());
		headers += "\nConnection: Closed";
		headers += "\n\n";
		response.headers = headers;
	}

	void Request::status404(response &response, string &requestedFile) {
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

	void Request::status405(response &response, string &method) {
		string errorHeaders = "HTTP/1.1 405 Method Not Allowed";
		errorHeaders += "\nContent-Type: text/html";
		response.binary = false;
		string body = htmlTemplate(
			"405 Method Not Allowed",
			"Request method '" + method + "' not allowed."
		);
		errorHeaders += "\nContent-Length: " + to_string(body.length());

		response.headers = errorHeaders + "\n\n";
		response.body = vector<char>(body.begin(), body.end());
	}

	void Request::status500(response &response) {
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

	void Request::logError(string line) {
		openlog("nginept", LOG_PID|LOG_CONS, LOG_DAEMON);
		syslog(LOG_ERR, line.c_str());
		closelog();
	}

	Request::request Request::parseRequest(string buffer) {
		request request;
		size_t start;

		// Get method. Only support GET and POST for the time being.
		size_t end = buffer.find(' ');
		string method = buffer.substr(0, end);
		if(strcmp(method.c_str(), "POST") == 0) {
			request.method = "POST";
		} else if(strcmp(method.c_str(), "GET") == 0) {
			request.method = "GET";
		} else {
			throw method.c_str();
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

	Request::requestContext Request::parseHeaders(string buffer) {
		requestContext rc;

		// First line is request itself (-1 to remove \n)
		size_t lineBreak = buffer.find('\n');
		string request = buffer.substr(0, lineBreak - 1);
		try {
			rc.request = parseRequest(request);
		} catch(const char* e) {
			throw e;
		}

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