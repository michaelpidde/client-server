#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iterator>
#include <vector>
#include <sys/stat.h>
#include "File.h"

using namespace std;

namespace Network {
	File::File() {}

	File::~File() {}

	vector<char> File::readStatic(string location, bool binary) {
		ifstream inFile;

		if(binary) {
			ifstream inFile(location, ios::binary);
		} else {
			ifstream inFile(location);
		}

		if(!inFile.good()) {
			throw "404";
		}

		vector<char> fileBuffer(
			(istreambuf_iterator<char>(inFile)),
			istreambuf_iterator<char>()
		);
		return fileBuffer;
	}

	string File::getFileExtension(string file) {
		regex regex("\\.([a-zA-Z]+)$");
		smatch match;
		string ext = "";
		if(regex_search(file, match, regex)) {
			ext = match[1];
		}
		return ext;
	}

	bool File::isValidDirectory(string directory) {
		struct stat check;
		if(stat(directory.c_str(), &check) == 0 && S_ISDIR(check.st_mode)) {
			return true;
		} else {
			return false;
		}
	}

	void File::writeLine(string path, string line) {
		ofstream outfile(path, ios::out | ios::app);
		if(outfile.good()) {
			outfile << line + "\n";
		}
		outfile.close();
	}
}