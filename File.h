#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <regex>

namespace Network {
	class File {
		public:
			File();
			~File();
			static std::vector<char> readStatic(std::string location, bool binary);
			static std::string getFileExtension(std::string file);
			static bool isValidDirectory(std::string directory);
			static void writeLine(std::string file, std::string line);
	};
}

#endif /*_SOCKET_H_*/