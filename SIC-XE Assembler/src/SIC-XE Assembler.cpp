#include<iostream>
#include<map>
#include<string>
#include<fstream>
#include<regex>

using namespace std;

struct struct_opcode {
	string opcode;
	int format;
};
map<string, struct_opcode> opTable;

void loadOpTable(string path) {
	string line;
	ifstream infile;
	infile.open(path);
	while (getline(infile, line)) {
		regex r("(\\w+)\\s+(\\w+)\\s+(\\w+)");
		smatch m;
		regex_search(line, m, r);
		opTable[m[1]].format = stoi(m[2]);
		opTable[m[1]].opcode = m[3];
		//cout << m[1] << "      " << opTable[m[1]].format<< "      " << opTable[m[1]].opcode << endl ;
	}
	infile.close();
}
int main() {
	loadOpTable("optable.txt");
	return 0;
}
