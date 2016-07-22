namespace Network {
	class File {
		public:
			File();
			~File();
			static std::vector<char> readStatic(std::string location, bool binary);
	};
}