#include<iostream>
#include<map>
#include<string>
#include<fstream>
#include<regex>
#include<vector>
#include<iterator>
#include <sstream>

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
	if(exists_test0(path)){
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
}

void write_listing_file() {
	ofstream file;
	file.open("ListingTable.txt");
	file << "Line no. Address Label    Mnemonic  Operand          Comments\n\n";
	map<unsigned int, listing_line>::iterator itr;
	int i = 0;
	bool flag = false;
	for (itr = listing_table.begin(); itr != listing_table.end(); itr++) {
		i++;
		if (flag) {
			unsigned int t = 0;
			for (t = 0; t < listing_table[i - 2].error.size(); t++) {
				file << "      ****Error: ";
				file << listing_table[i - 2].error.at(t);
				file << "\n";
			}
			flag = false;
		}
		else if (!listing_table[i - 1].error.empty()) {
			flag = true;
		}
		if (i < 10) {
			file << i;
			file << "        ";
		} else if (i < 100) {
			file << i;
			file << "       ";
		} else if (i < 1000) {
			file << i;
			file << "      ";
		}
		std::stringstream ss;
		ss << std::hex << itr->second.address; // int decimal_value
		std::string res(ss.str());

		int temp = 6 - res.length();
		while (temp > 0) {
			res = "0" + res;
			temp--;
		}
		file << res;
		file << "  ";
		file << itr->second.label;
		unsigned int spaces2 = 9-itr->second.label.size();
		while(spaces2 > 0){
			file << " ";
			spaces2--;
		}
		if (itr->second.isFormat4){
			file << '+'<<itr->second.mnemonic;
		}else{
			file << itr->second.mnemonic;
		}
		unsigned int spaces1 = 10 - itr->second.mnemonic.size();
		if(itr->second.isFormat4){
			spaces1--;
		}
		while (spaces1 > 0) {
			file << " ";
			spaces1--;
		}

		file << itr->second.operand;
		unsigned int spaces = 17 - itr->second.operand.size();
		while (spaces > 0) {
			file << " ";
			spaces--;
		}
		file << itr->second.comment;
		file << "\n";
	}
	file.close();
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
				if (opTable[mnemonic].format == 3 && current_line.isFormat4) {
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

		++current_line_number;
		if(listing_table.find(current_line_number) == listing_table.end()){
			listing_table[current_line_number - 1].error.push_back("There is no End directive");
			break;
		}
		current_line = listing_table[current_line_number];
	}
	listing_table[current_line_number].address = LOCCTR;
	program_length = LOCCTR - starting_address;

}

int main() {
	//Enter "pass1 <input-file-name>" to start
	//pass1 input.txt

	loadOpTable("optable.txt");
	bool error = true;
	while(error){
		cout << "Enter 'pass1 <input-file-name>' to start, Ex : pass1 input.txt" <<endl;
		string input;
		getline(cin, input);
		smatch m;
		regex r("^pass1\\s+(\\S+)$");
		regex_search(input, m, r);
		if (m.size() > 0) {
			error = false;
			pass1_Algorithm(m[1].str());
			write_listing_file();
		}else{
			cout << "File name format is not correct" <<endl<<endl;
		}
	}
	return 0;
}
