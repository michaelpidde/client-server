// This first one is used for GNU so the remove function works.
#include <algorithm>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Config.h"

using namespace std;

namespace Network {
	Config::Config() {
		readDefault();
	};

	Config::~Config() {};

	void Config::readDefault() {
		cout << "READING CONFIG" << endl;
		ifstream inFile("config-default.conf");

		if(!inFile.good()) {
			cout << "Error: Could not load config-default.conf\n";
			exit(EXIT_FAILURE);
		}

		// Parse settings into local variables.
		string line;
		while(getline(inFile, line)) {
			istringstream singleLine(line);
			string key;

			// This is really stupid... Don't do this for real.
			if(getline(singleLine, key, '=')) {
				string value;
				if(getline(singleLine, value)) {
					// Remove any newline or carriage returns from the end of the value.
					value.erase(remove(value.begin(), value.end(), '\n'), value.end());
					value.erase(remove(value.begin(), value.end(), '\r'), value.end());

					if(key.compare("defaultFile") == 0) {
						this->defaultFile = value;
					} else if(key.compare("logPath") == 0) {
						this->logPath = value;
					} else if(key.compare("host") == 0) {
						this->host.host = value;
					} else if (key.compare("sysPath") == 0) {
						this->host.sysPath = value;
					}
				}
			}
		}
	}

	string Config::pathFormat(string &path) {
		char c = path.back();
		if(c == '/') {
			return path;
		} else {
			return path + "/";
		}
	}

	knownHost Config::getHost() {
		return this->host;
	}

	string Config::getDefaultFile() {
		return this->defaultFile;
	}

	string Config::getLogPath() {
		return pathFormat(this->logPath);
	}
}