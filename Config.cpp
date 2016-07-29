#include <iostream>
#include <fstream>
#include <string>
#include "Config.h"

using namespace std;

namespace Network {
	Config::Config() {
		readDefault();
	};

	Config::~Config() {};

	void Config::readDefault() {
		ifstream inFile("config-default.conf");

		if(!inFile.good()) {
			cout << "Error: Could not load config-default.conf\n";
			exit(EXIT_FAILURE);
		}

		vector<char> fileBuffer(
			(istreambuf_iterator<char>(inFile)),
			istreambuf_iterator<char>()
		);

		// Parse settings into local variables.
	}
}