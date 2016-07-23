#include <vector>

namespace Network {
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

	class Request {
		public:
			Request();
			~Request();
			response handle(std::string buffer);
			requestContext parseHeaders(std::string buffer);
			request parseRequest(std::string buffer);
			std::vector<std::string> binTypes {
				"jpg","jpeg",
				"gif",
				"png",
				"bmp",
				"ico"
			};
			void error500(response &response);
	};
}