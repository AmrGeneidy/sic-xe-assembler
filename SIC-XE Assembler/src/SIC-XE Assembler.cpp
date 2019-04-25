#include<iostream>
#include<map>
#include<string>
#include<fstream>
#include<regex>
#include<vector>
#include<iterator>

#include"assemblerdata.h"
#include"utility.h"

using namespace std;

map<string, struct_opcode> opTable;
map<unsigned int, listing_line> listing_table;
map<string, unsigned int> symbol_table;

unsigned int starting_address;
unsigned int program_length;

unsigned int current_line_number;
unsigned int LOCCTR;
listing_line current_line;

bool is_mnemonicOrDirective(string x);
bool parse_label(string x, int i);
bool parse_mnemonic(string x, int i);
bool parse_operand(string x, int i);
bool parse_instruction(string x[], int i);
void loadOpTable(string path);
void build_listing_table(string path);
void write_listing_file();
void pass1_Algorithm(string codePath);

bool is_mnemonicOrDirective(string x) {
	string temp = getUpperVersion(x);
	if ((opTable.find(temp) != opTable.end())||isDirective(temp)) {
		return true;
	}
	return false;
}
bool parse_label(string x, int i) {
	smatch m;
	regex r("^([A-Za-z]\\w*)$");
	regex_search(x, m, r);
	if (m.size() > 0) {
		listing_table[i].label = m[1].str();
		return true;
	}
	return false;
}

bool parse_mnemonic(string x, int i) {
	smatch m;
	regex r("^(\\+?(\\w+))$");
	regex_search(x, m, r);
	if (m.size() > 0) {
		if (m[1].str().at(0) == '+') {
			listing_table[i].isFormat4 = true;
		}
		listing_table[i].mnemonic = m[2].str();
		return true;
	}
	return false;
}
bool parse_operand(string x, int i) {
	smatch m;
	regex r("^(\\*|([#@]?\\w+)|(\\w+(\\,|\\+|\\-|\\*|\\/)\\w+))$");
	//regex r("^(\\*|([#@]?\\w+)|(\\w+[\\,\\+\\-\\*\\/]\\w+))$");
	regex_search(x, m, r);
	if (m.size() > 0) {
		listing_table[i].operand = m[1].str();
		return true;
	}
	return false;
}

//3 cases
bool parse_instruction(string x[], int i) {
	bool noError = true;
	//case 1 mnemonic only
	if (x[0].empty() && !x[1].empty() && x[2].empty()) {
		noError = parse_mnemonic(x[1], i);
	} //case 2 label, mnemonic and operand exists
	else if (!x[0].empty() && !x[1].empty() && !x[2].empty()) {
		noError = parse_label(x[0], i);
		noError = parse_mnemonic(x[1], i);
		noError = parse_operand(x[2], i);
	} //case 3 (label and mnemonic) OR (mnemonic and operand)
	else if (!x[0].empty() && !x[1].empty() && x[2].empty()) {
		//label and mnemonic
		if(is_mnemonicOrDirective(x[1])){
			noError = parse_label(x[0], i);
			noError = parse_mnemonic(x[1], i);
		}//mnemonic and operand
		else if(is_mnemonicOrDirective(x[0])){
			noError = parse_mnemonic(x[0], i);
			noError = parse_operand(x[1], i);
		}else {
			return false;
		}
	}else{
		return false;
	}
	return noError;

}

void loadOpTable(string path) {
	string line;
	ifstream infile;
	infile.open(path);
	while (getline(infile, line)) {
		regex r("(\\w+)\\s+(\\w+)\\s+(\\w+)");
		smatch m;
		regex_search(line, m, r);
		opTable[m[1].str()].format = stoi(m[2].str());
		opTable[m[1].str()].opcode = stoi(m[3].str(), 0, 16);
	}
	infile.close();
}

void build_listing_table(string path) {
	string line;
	ifstream infile;
	infile.open(path);
	int i = 0;
	while (getline(infile, line)) {
		cout << line << endl;
		regex rComment("^(\\.)(.*)");
		smatch m;
		regex_search(line, m, rComment);
		if (m.size() > 0 && m.position(0) == 0) {
			listing_table[i].isAllComment = true;
			listing_table[i].comment = m[0].str();
			i++;
			continue;
		} else {
			//ensure that label, mnemonic and operand don't start with "." (comment filter)
			regex rInstruction(
					"^\\s*(([^\\.]\\S*)\\s+)?([^\\.]\\S*)\\s*(\\s+([^\\.]\\S*))?\\s*(\\s+(\\..*))?$");
			regex_search(line, m, rInstruction);
			if (m.size() > 0) {
				listing_table[i].comment = m[7].str();
				string instruction[] = { m[2].str(), m[3].str(), m[5].str() };
					if(!parse_instruction(instruction, i)){
					listing_table[i].error.insert(listing_table[i].error.begin(),
							"Invalid Instruction");
				}
			} else {
				listing_table[i].error.insert(listing_table[i].error.begin(),
						"Invalid Instruction");
			}
		}
		i++;
	}
	infile.close();
}

void write_listing_file() {
	map<unsigned int, listing_line>::iterator itr;
	int i = 0;
	for (itr = listing_table.begin(); itr != listing_table.end(); itr++) {
		i++;
		cout << i << endl;
		cout << "address: " << itr->second.address << endl;
		cout << "label: " << itr->second.label << endl << "mnemonic: "
				<< itr->second.mnemonic << endl;
		cout << "operand: " << itr->second.operand << endl << "comment: "
				<< itr->second.comment << endl;
//		cout<<itr->second.error.empty()?"":itr->second.error[0];
	}

}

void pass1_Algorithm(string codePath) {
	build_listing_table(codePath);
	current_line_number = 0;

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
		//assign address to the line
		listing_table[current_line_number].address = LOCCTR;

		//process the line if not a comment
		if (!current_line.isAllComment) {
			//process the label field
			if (!current_line.label.empty()) {
				//save the label in symbol table in upper case form
				string label = getUpperVersion(current_line.label);
				if (symbol_table.find(label) != symbol_table.end()) {
					listing_table[current_line_number].error.push_back(
							"symbol '" + current_line.label
									+ "' is already defined");
				} else {
					symbol_table[label] = LOCCTR;
				}
			}

			//process the mnemonic
			string mnemonic = getUpperVersion(current_line.mnemonic);

			if (opTable.find(mnemonic) != opTable.end()) {
				//not directive
				if (opTable[mnemonic].format == 3
						&& current_line.isFormat4) {
					LOCCTR += 4;
				} else if (opTable[mnemonic].format == 2
						&& current_line.isFormat4) {
					listing_table[current_line_number].error.push_back(
							"Can't use format 4 with mnemonic "
									+ current_line.mnemonic);
				} else {
					LOCCTR += opTable[mnemonic].format;
				}
			} else if (handleDirective(current_line)) {

			} else {
				listing_table[current_line_number].error.push_back(
						"Invalid operation code");
			}
		}				//end line process
		current_line = listing_table[++current_line_number];
	}
	listing_table[current_line_number].address = LOCCTR;
	program_length = LOCCTR - starting_address;

}

int main() {
	loadOpTable("optable.txt");
	pass1_Algorithm("input.txt");
	write_listing_file();
	return 0;
}
