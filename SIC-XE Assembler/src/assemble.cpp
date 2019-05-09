#include<iostream>
#include"assemblerdata.h"
#include"utility.h"

using namespace std;

int base = -1;
map<string, string> registers;
string objectCode;
unsigned int current_line_number;
listing_line current_line;

void loadRegisters() {
	registers["A"] = "0000";
	registers["X"] = "0001";
	registers["L"] = "0010";
	registers["B"] = "0011";
	registers["S"] = "0100";
	registers["T"] = "0101";
	registers["F"] = "0110";
	registers["PC"] = "1000";
	registers["SW"] = "1001";
}

bool handleFormat2(string mnemonic, string operand) {
	objectCode = hextobin(opTable[mnemonic].opcode);
	smatch m;
	regex r("^(\\w+)(\\,(\\w+))?$");
	regex_search(operand, m, r);
	if (m.size() > 0 && registers.find(m[1].str()) != registers.end()) {
		//register 1 exist
		objectCode.append(registers[m[1].str()]);
		if (m[3].str().empty()) {
			//no register 2
			objectCode.append("0000");
		} else {
			if (registers.find(m[3].str()) != registers.end()) {
				//register 2 exist
				objectCode.append(registers[m[3].str()]);
			} else {
				return false;
			}
		}
	} else {
		return false;
	}
	objectCode = bintohex(objectCode);
	return true;
}

bool isOperandToTargetAddress(string operand) {
	if (operand.empty()) {
		return false;
	} else if (symbol_table.find(operand) != symbol_table.end()) {
		address = symbol_table[operand].address;
	} else if (iequals("*", operand)) {
		address = LOCCTR;
	} else {
		if (isRelocatable(operand)) {
			return true;
		} else if (isAbsluteExp(operand)) {
			return true;
		}
		try {
			int z = stoi(operand);
			address = z;
		} catch (invalid_argument& e) {
			return false;
		}
	}
	return true;
}

//return false if there is an error in operand
bool instructionToObjectCode(unsigned int line_number) {
	objectCode.clear();
	loadRegisters();
	string operand = getUpperVersion(listing_table[line_number].operand);
	string mnemonic = getUpperVersion(listing_table[line_number].mnemonic);
	unsigned int format =
			listing_table[line_number].isFormat4 == true ?
					4 : opTable[mnemonic].format;
	if (format == 2) {
		if (handleFormat2(mnemonic, operand)) {
			return true;
		} else {
			listing_table[line_number].error.push_back(
					"Couldn't process format 2 !!");
			return false;
		}

	}
	//handle special case RSUB has no operand
	if (mnemonic == "RSUB") {
		objectCode = bintohex(objectCode.append("110000"));
		objectCode.append("000");
		if (format == 4) {
			objectCode.append("00");
		}
		return true;
	}
	objectCode = hextobin(opTable[mnemonic].opcode).substr(0, 6);
	string last2chars = "";
	last2chars.push_back(operand[operand.size() - 2]);
	last2chars.push_back(operand[operand.size() - 1]);
	if (iequals(last2chars, ",X")) {
		//indexing nix = 111
		objectCode.append("111");
		operand = operand.substr(0, operand.size() - 2);
	} else if (operand[0] == '#') {
		//immediate nix = 010
		objectCode.append("010");
		operand = operand.substr(1, operand.size() - 1);
		if (is_number(operand)) {
			//special case operand is #int ex: #3
			if (format == 4) {
				if (stoi(operand) < 0 || stoi(operand) > 1048575) {
					//1048575 dec = fffff hex
					listing_table[line_number].error.push_back(
							"Operand Can't be greater than 0XFFFFF or less than 0 in format 4 !!");
					return false;
				}
				//bpe = 001
				objectCode.append("001");
				objectCode = bintohex(objectCode);
				objectCode.append(intToBinaryString(stoi(operand), 5));
			} else {
				if (stoi(operand) < 0 || stoi(operand) > 4095) {
					//4095 dec = fff hex
					listing_table[line_number].error.push_back(
							"Operand Can't be greater than 0XFFF or less than 0 in format 3 !!");
					return false;
				}
				//bpe = 000
				objectCode.append("000");
				objectCode = bintohex(objectCode);
				objectCode.append(intToBinaryString(stoi(operand), 3));
			}
			return true;
		}
		//TODO evaluate expression
		//if(/*expression*/)
	} else if (operand[0] == '@') {
		//indirect nix = 100
		objectCode.append("100");
		operand = operand.substr(1, operand.size() - 1);
	} else {
		//simple addressing with no indexing nix = 110
		objectCode.append("110");
	}
	int TA;
	if (isOperandToTargetAddress(operand)) {
		TA = address;
	} else {
		listing_table[line_number].error.push_back("Invalid operand !!");
		return false;
	}
	if (format == 4) {
		//TODO modification record
		objectCode.append("001");
		objectCode.append(bintohex(intToBinaryString(TA, 5)));
	} else {
		int disp = TA - (LOCCTR + format);
		if (-2048 <= disp && disp <= 2047) {
			//PC-relative bpe = 010
			objectCode.append("010");
		} else if (base >= 0) {
			//base available
			disp = TA - base;
			if (0 <= disp && disp <= 4095) {
				//base relative bpe = 100
				objectCode.append("100");
			} else {
				listing_table[line_number].error.push_back(
						"Can't Process the address!!");
				return false;
			}
		}
		objectCode.append(bintohex(intToBinaryString(disp, 3)));
	}
	if (objectCode.length() != 6 && format == 3) {
		listing_table[line_number].error.push_back(
				"Error in operand in format 3 !!");
		return false;
	} else if (objectCode.length() != 8 && format == 4) {
		listing_table[line_number].error.push_back(
				"Error in operand in format 4 !!");
		return false;
	} else {
		return true;
	}

}
void pass2() {

	current_line_number = 0;

	//skip the comments
	while (listing_table[current_line_number].isAllComment) {
		current_line_number++;
	}

	current_line = listing_table[current_line_number];
	if (!current_line.error.empty()) {
		return;
	}
	ofstream file;
	file.open("ObjectCodeFile.txt");
	file << "H";

	//check start mnemonic (Program name)
	if (iequals(current_line.mnemonic, "START")) {
		if (current_line.label.size() > 6) {
			current_line.error.push_back(
					"Program name is more than 6 characters");
		} else {
			int spacesNum = 6 - current_line.label.size();
			file << current_line.label;
			for (int i = 0; i < spacesNum; ++i) {
				file << " ";
			}
		}
		current_line = listing_table[++current_line_number];
	} else {
		file << "      ";
	}
	LOCCTR = starting_address;
	file << bintohex(intToBinaryString(starting_address, 6));
	file << bintohex(intToBinaryString(program_length, 6));
	file << "\n";
	//max length of tRecord = 30 bytes (0x1E) = 60 hex digits
	int tRecordLength = 0;		//length by hex digits
	int tRecordStart = starting_address;
	vector<string> tRecords;
	string tempRecord;

	while (!iequals(current_line.mnemonic, "END") && current_line.error.empty()) {
		LOCCTR = current_line.address;
		if (tRecordStart == -1) {
			tRecordStart = LOCCTR;
		}

		//process the line if not a comment
		if (!current_line.isAllComment) {
			string mnemonic = getUpperVersion(current_line.mnemonic);
			if (opTable.find(mnemonic) != opTable.end()) {
				//not directive
				if (instructionToObjectCode(current_line_number)){
					tRecordLength += objectCode.size();
					tRecords.push_back(objectCode);
				}
			}else{
				//directive
				if(iequals(current_line.mnemonic, "NOBASE")){
					base = -1;
				}else if (iequals(current_line.mnemonic, "BASE")){
					if(!handleBasePass2(current_line_number)) {
						current_line.error.push_back("Error in BASE operand");
					}
				}else if (iequals(current_line.mnemonic, "WORD")){
					objectCode = wordObCode(current_line_number);
					tRecordLength += objectCode.size();
					tRecords.push_back(objectCode);
				}else if (iequals(current_line.mnemonic, "BYTE")){
					objectCode = byteObCode(current_line_number);
					tRecordLength += objectCode.size();
					tRecords.push_back(objectCode);
				}
			}
			if (tRecordLength > 60) {
				file << "T";
				file << bintohex(intToBinaryString(tRecordStart, 6));
				tempRecord = tRecords.back();
				tRecords.erase(tRecords.end() - 1);
				tRecordLength -= tempRecord.size();
				file << bintohex(intToBinaryString(tRecordLength / 2, 2));
				for (int i = 0; i < tRecords.size(); i++) {
					file << tRecords.at(i);
				}
				file << "\n";
				tRecords.clear();
				tRecords.push_back(tempRecord);
				tRecordStart = LOCCTR;
				tRecordLength = tempRecord.size();
			} else if (iequals(current_line.mnemonic, "RESW")
					|| iequals(current_line.mnemonic, "RESB")
					|| iequals(current_line.mnemonic, "ORG")) {
				if (!tRecords.empty()) {
					file << "T";
					file << bintohex(intToBinaryString(tRecordStart, 6));
					file << bintohex(intToBinaryString(tRecordLength / 2, 2));
					for (int i = 0; i < tRecords.size(); i++) {
						file << tRecords.at(i);
					}
					file << "\n";
					tRecords.clear();
					tRecordLength = 0;
				}
				//start from the next iteration
				tRecordStart = -1;
			}

		}
		current_line = listing_table[++current_line_number];
	}
	if (iequals(current_line.mnemonic, "END")) {
		file << "T";
		file << bintohex(intToBinaryString(tRecordStart, 6));
		file << bintohex(intToBinaryString(tRecordLength / 2, 2));
		for (int i = 0; i < tRecords.size(); i++) {
			file << tRecords.at(i);
		}
		file << "\n";
		file << "E";
		if(!current_line.operand.empty()){
			if(isOperandToTargetAddress(current_line.operand)){
				file << bintohex(intToBinaryString(address, 6));
			}else{
				current_line.error.push_back("Error in End operand !!");
			}
		}else{
			file << "000000";
		}
	}
	file.close();
}

int main() {
	runPass1("input.txt");
	pass2();
	write_listing_file("pass2_error_report.txt");
}
