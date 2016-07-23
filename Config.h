#include "knownhost.h"

namespace Network {
	class Config {
		public:
			Config();
			~Config();
			static bool isValidDirectory(std::string directory);
	};
}