#include<iostream>
#include<map>
#include<string>
#include<fstream>
#include<regex>

using namespace std;

struct struct_opcode {
	string opcode;
	int format;
	struct_opcode() {
		opcode = "undefined";
		format = 0;
	}
};
std::map<std::string, struct_opcode> opTable;

void loadOpTable() {
	string line;
	ifstream infile;
	infile.open("optable.txt");
	while (getline(infile, line)) {
		std::regex r("(\\w+)\\s+(\\w+)\\s+(\\w+)");
		std::smatch m;
		std::regex_search(line, m, r);
		std::istringstream iss (m[2]);
		int x;
		iss >> x;
		opTable[m[1]].format = x;
		opTable[m[1]].opcode = m[3];
	}
	infile.close();
}
int main() {
//	std::string str("ADD         3      18");
//	std::regex r("(\\w+)\\s+(\\w+)\\s+(\\w+)"); // entire match will be 2 numbers
//	std::smatch m;
//	std::regex_search(str, m, r);
//
//	for (unsigned int j = 1; j < m.size(); j++) {
//		std::cout << m[j] << std::endl;
//	}

	loadOpTable();
	return 0;
}
