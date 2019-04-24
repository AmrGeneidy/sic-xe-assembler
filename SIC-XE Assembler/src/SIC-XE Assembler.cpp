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
			listing_table[i].comment = m[0];
			i++;
			continue;
		} else {
			regex rInstruction(
					"^\\s*(\\w+(\\s+)){0,1}?(\\+?\\w+)\\s*(\\s+(\\*|[#@=]?\\w+))?\\s*(\\s+(\\..*))?$");

			regex_search(line, m, rInstruction);
			if (m.size() > 0) {
				listing_table[i].isAllComment = false;
				//TODO handle case label and mnemonic only
				listing_table[i].label = m[1];
				listing_table[i].mnemonic = m[3];
				listing_table[i].operand = m[5];
				listing_table[i].comment = m[7];
				//cout << "hello"<<endl;
				//cout <<listing_table[i].label <<listing_table[i].mnemonic <<" "<< listing_table[i].operand <<" "<<listing_table[i].comment<<endl;
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
	loadOpTable("optable.txt");
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
				if (symbol_table.find(current_line.label) != symbol_table.end()
						&& !iequals(current_line.mnemonic, "EQU")) {
					listing_table[current_line_number].error.push_back(
							"symbol '" + current_line.label
									+ "' is already defined");
				} else {
					symbol_table[current_line.label] = LOCCTR;
				}
			}

			//process the mnemonic
			if (opTable.find(current_line.mnemonic) != opTable.end()) {
				//not directive
				if (opTable[current_line.mnemonic].format == 3
						&& current_line.isFormat4) {
					LOCCTR += 4;
				} else if (opTable[current_line.mnemonic].format == 2
						&& current_line.isFormat4) {
					listing_table[current_line_number].error.push_back(
							"Can't use format 4 with mnemonic "
									+ current_line.mnemonic);
				} else {
					LOCCTR += opTable[current_line.mnemonic].format;
				}
			} else if (handleDirective(current_line)) {

			} else {
				listing_table[current_line_number].error.push_back(
						"Invalid operation code");
			}
		}				//end line process
		current_line = listing_table[++current_line_number];
	}
	program_length = LOCCTR - starting_address;

}

int main() {
	cout << "starting" << endl;
	pass1_Algorithm("input.txt");
//	cout << listing_table[5].address << " " << listing_table[5].label << " "
//			<< listing_table[5].mnemonic << " " << listing_table[5].operand
//			<< endl;
	write_listing_file();
	return 0;
}
