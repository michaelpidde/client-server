#ifndef _REQUEST_H_
#define _REQUEST_H_

#include <vector>
#include "Config.h"
#include "knownhost.h"

namespace Network {
	class Request {
		public:
			Request(Config &config);
			~Request();
			struct request {
				std::string method;
				std::string resource;
				std::string httpVersion;
			};

			struct response {
				std::string headers;
				std::vector<char> body;
				bool binary;
			};

			struct requestContext {
				Request::request request;
				std::string host = "";
				std::string accept = "";
				std::string encoding = "";
				std::string agent = "";
				std::string referer = "";
			};
			std::vector<std::string> binTypes {
				"jpg","jpeg",
				"gif",
				"png",
				"bmp",
				"ico"
			};
			response handle(std::string buffer);
		private:
			Config *config;
			knownHost getKnownHost(std::string host);
			requestContext parseHeaders(std::string buffer);
			request parseRequest(std::string buffer);
			std::string htmlTemplate(std::string title, std::string body);
			void logError(std::string line);
			void status200(response &response, bool binary, std::string extension);
			void status404(response &response, std::string &requestedFile);
			void status405(response &response, std::string &method);
			void status500(response &response);
	};
}

#endif /*_REQUEST_H_*/