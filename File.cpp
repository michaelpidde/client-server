#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iterator>
#include <vector>
#include "File.h"

using namespace std;

namespace Network {
	File::File() {}

	File::~File() {}

	vector<char> File::readStatic(string location, bool binary) {
		ifstream inFile;
		if(binary) {
			inFile = ifstream(location, ios::binary);
		} else {
			inFile = ifstream(location);
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
		cout << ext << endl;
		return ext;
	}
}