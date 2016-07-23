#include <vector>
#include "knownhost.h"

namespace Network {
	class Request {
		public:
			Request();
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
				request request;
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
			knownHost getKnownHost(std::string host);
			response handle(std::string buffer);
			requestContext parseHeaders(std::string buffer);
			request parseRequest(std::string buffer);
			std::string htmlTemplate(std::string title, std::string body);
			void status200(response &response, bool binary, std::string extension);
			void status404(response &response, std::string &requestedFile);
			void status500(response &response);
	};
}