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
			std::vector<knownHost> getHosts();
		private:
			std::string defaultFile;
			std::string logPath;
			std::vector<knownHost> hosts;
			void readDefault();
	};
}

#endif /*_CONFIG_H_*/