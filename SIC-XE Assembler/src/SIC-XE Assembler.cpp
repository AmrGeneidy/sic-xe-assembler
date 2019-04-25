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
map<string, symbol_struct> symbol_table;

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
		opTable[m[1].str()].format = stoi(m[2].str());
		opTable[m[1].str()].opcode = stoi(m[3].str(), 0, 16);
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
					symbol_table[label].address = LOCCTR;
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
