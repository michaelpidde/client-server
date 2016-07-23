#include <sys/stat.h>
#include <string>
#include "Config.h"

using namespace std;

namespace Network {
	Config::Config() {};

	Config::~Config() {};

	bool Config::isValidDirectory(string directory) {
		struct stat check;
		if(stat(directory.c_str(), &check) == 0 && S_ISDIR(check.st_mode)) {
			return true;
		} else {
			return false;
		}
	}
}