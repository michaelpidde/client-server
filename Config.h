#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <vector>
#include "knownhost.h"

namespace Network {
	class Config {
		public:
			Config();
			~Config();
			std::string getDefaultFile();
			std::string getLogPath();
			knownHost getHost();
			static std::string pathFormat(std::string &path);
		private:
			std::string defaultFile;
			std::string logPath;
			knownHost host;
			void readDefault();

	};
}

#endif /*_CONFIG_H_*/