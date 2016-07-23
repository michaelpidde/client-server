#include <fstream>
#include <sstream>
#include <iterator>
#include <vector>
#include "File.h"

using namespace std;

namespace Network {
	File::File() {}

	File::~File() {}

	std::vector<char> File::readStatic(string location, bool binary) {
		ifstream inFile;
		if(binary) {
			inFile = ifstream(location, ios::binary);
		} else {
			inFile = ifstream(location);
		}

		if(!inFile.good()) {
			throw "404";
		}

		std::vector<char> fileBuffer(
			(istreambuf_iterator<char>(inFile)),
			istreambuf_iterator<char>()
		);
		return fileBuffer;
	}
}