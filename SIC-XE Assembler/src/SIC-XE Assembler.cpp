#include<iostream>
#include<map>
#include<string>
#include<fstream>
#include<regex>
#include<vector>

#include"assemblerdata.h"

using namespace std;

map<string, struct_opcode> opTable;
map<unsigned int, listing_line> listing_table;
map<string, unsigned int> symbol_table;

unsigned int starting_address;
unsigned int program_length;

unsigned int current_line_number;
unsigned int LOCCTR;
listing_line current_line;
bool handleByte(listing_line x);
bool handleWord(listing_line x);
bool handleRes(listing_line x, int increase_val);
bool handleOrg(listing_line x);
bool handleEqu(listing_line x);
bool handleBase(listing_line x);
bool handleDirective(listing_line x);
void loadOpTable(string path) {
	string line;
	ifstream infile;
	infile.open(path);
	while (getline(infile, line)) {
		regex r("(\\w+)\\s+(\\w+)\\s+(\\w+)");
		smatch m;
		regex_search(line, m, r);
		opTable[m[1]].format = stoi(m[2]);
		opTable[m[1]].opcode = stoi(m[3], 0, 16);
		//TODO delete this line
		//cout << m[1] << "      " << opTable[m[1]].format<< "      " << opTable[m[1]].opcode << endl ;
	}
	infile.close();
}

void pass1_Algorithm(string codePath) {
	//TODO load listing_table
	loadOpTable("optable.txt");
	current_line_number = 1;

	//skip the comments
	while (listing_table[current_line_number].isAllComment) {
		current_line_number++;
	}

	current_line = listing_table[current_line_number];
	//check start addressing
	if (iequals(current_line.mnemonic, "START")) {
		starting_address = stoi(current_line.operand, 0, 16);
		listing_table[current_line_number].address = starting_address;
		current_line = listing_table[++current_line_number];
	} else {
		starting_address = 0;
	}
	LOCCTR = starting_address;

	//reading code loop
	while (!iequals(current_line.mnemonic, "END")) {
		//process the line if not a comment
		if (!current_line.isAllComment) {
			//assign address to the line
			listing_table[current_line_number].address = LOCCTR;

			//process the label field
			if (!current_line.label.empty()) {
				if (symbol_table.find(current_line.label)
						!= symbol_table.end()) {
					listing_table[current_line_number].error.push_back("symbol '"
							+ current_line.label + "' is already defined") ;
				} else {
					symbol_table[current_line.label] = LOCCTR;
				}
			}

			//process the mnemonic
			if(opTable.find(current_line.mnemonic) != opTable.end()){
				//not directive
				if (opTable[current_line.mnemonic].format == 3 && current_line.isFormat4) {
					LOCCTR += 4;
				}else if (opTable[current_line.mnemonic].format == 2 && current_line.isFormat4) {
					listing_table[current_line_number].error.push_back("Can't use format 4 with mnemonic " + current_line.mnemonic) ;
				}else {
					LOCCTR += opTable[current_line.mnemonic].format;
				}
			}else if(false /*TODO handle all the directives*/){

			}else{
				listing_table[current_line_number].error.push_back("Invalid operation code") ;
			}
		}//end line process
		current_line = listing_table[++current_line_number];
	}
	program_length = LOCCTR - starting_address;

}
int main() {
	loadOpTable("optable.txt");
	return 0;
}
bool handleDirective(listing_line x) {
	if (iequals(x.mnemonic, "byte")) {
		return handleByte(x);
	} else if (iequals(x.mnemonic, "word")) {
		return handleWord(x);
	} else if (iequals(x.mnemonic, "resw")) {
		return handleRes(x, 3);
	} else if (iequals(x.mnemonic, "resb")) {
		return handleRes(x, 1);
	} else if (iequals(x.mnemonic, "equ")) {
		return handleEqu(x);
	} else if (iequals(x.mnemonic, "org")) {
		return handleOrg(x);
	} else if (iequals(x.mnemonic, "base")) {
		return handleBase(x);
	} else if (iequals(x.mnemonic, "nobase")) {
		if (x.operand.empty() && x.label.empty()) {
		}
	} else {
		return false;
	}
	return true;
}
bool handleByte(listing_line x) {
	if (x.operand.size() < 3) {
		return false;
	}
	//TODO maxLength
	if (x.operand[0] == 'x' || x.operand[0] == 'X') {
		if (x.operand[1] == '\'' && x.operand[x.operand.size() - 1] == '\'') {
			try {
				stoi(x.operand, 0, 16);
			} catch (invalid_argument& e) {
				return false;
			}
			if (!x.label.empty()) {
				if (symbol_table.find(x.label) != symbol_table.end()) {
					return false;
				}
				symbol_table[x.label] = LOCCTR;
			}
			LOCCTR += (x.operand.size() - 2) / 2;
			return true;
		}
	} else if (x.operand[0] == 'c' || x.operand[0] == 'C') {
		if (x.operand[1] == '\'' && x.operand[x.operand.size() - 1] == '\'') {
			if (!x.label.empty()) {
				if (symbol_table.find(x.label) != symbol_table.end()) {
					return false;
				}
				symbol_table[x.label] = LOCCTR;
			}
			LOCCTR += x.operand.size() - 3;
			return true;
		}
	}
	return false;
}
bool handleWord(listing_line x) {
	//TODO handle operand 1,2,3 & #
	try {
		stoi(x.operand);
	} catch (invalid_argument& e) {
		return false;
	}
	if (!x.label.empty()) {
		if (!x.label.empty()) {
			if (symbol_table.find(x.label) != symbol_table.end()) {
				return false;
			}
			symbol_table[x.label] = LOCCTR;
		}
		symbol_table[x.label] = LOCCTR;
	}
	LOCCTR += 3;
	return true;
}
bool handleRes(listing_line x, int increase_val) {
	try {
		int z = stoi(x.operand);
		if (!x.label.empty()) {
			if (!x.label.empty()) {
				if (symbol_table.find(x.label) != symbol_table.end()) {
					return false;
				}
				symbol_table[x.label] = LOCCTR;
			}
			symbol_table[x.label] = LOCCTR;
		}
		LOCCTR += increase_val * z;
	} catch (invalid_argument& e) {
		return false;
	}
	return true;
}

bool handleOrg(listing_line x) {
	if (!x.label.empty()) {
		return false;
	} else if (symbol_table.find(x.operand) != symbol_table.end()) {
		LOCCTR = symbol_table[x.operand];
	} else {
		try {
			int z = stoi(x.operand);
			LOCCTR = z;
		} catch (invalid_argument& e) {
			return false;
		}
	}
	return true;
}

bool handleEqu(listing_line x) {
	if (x.label.empty()) {
		return false;
	} else if (symbol_table.find(x.operand) != symbol_table.end()) {
		symbol_table[x.label] = symbol_table[x.operand];
	} else if (iequals("*", x.operand)) {
		symbol_table[x.label] = LOCCTR;
	} else {
		try {
			int z = stoi(x.operand);
			symbol_table[x.label]= z;
		} catch (invalid_argument& e) {
			return false;
		}
	}
	return true;

}

bool handleBase(listing_line x){
	if (!x.label.empty()) {
		return false;
	}else if (symbol_table.find(x.operand) != symbol_table.end()) {

	}else if (iequals("*", x.operand)) {

	} else {
		try {
			stoi(x.operand);
		} catch (invalid_argument& e) {
			return false;
		}
	}
	return true;
}
