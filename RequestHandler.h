namespace Network {
	struct request {
		std::string method;
		std::string resource;
		std::string httpVersion;
	};

	struct requestContext {
		request request;
		std::string host = "";
		std::string accept = "";
		std::string encoding = "";
		std::string agent = "";
		std::string referer = "";
	};

	class RequestHandler {
		public:
			RequestHandler();
			~RequestHandler();
			std::string handle(std::string buffer);
			requestContext parseHeaders(std::string buffer);
			request parseRequest(std::string buffer);
	};
}