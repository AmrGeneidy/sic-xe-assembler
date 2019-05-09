#include<iostream>

#include"assemblerdata.h"
#include"utility.h"

using namespace std;

int base = -1;
map<string, string> registers;
string objectCode;
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
	loadRegisters();
	string operand = getUpperVersion(listing_table[line_number].operand);
	string mnemonic = getUpperVersion(listing_table[line_number].mnemonic);
	unsigned int format = listing_table[line_number].isFormat4 == true ? 4 : opTable[mnemonic].format;
	if (format == 2) {
		if(handleFormat2(mnemonic, operand)){
			return true;
		}else {
			listing_table[line_number].error.push_back("Couldn't process format 2 !!");
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
					listing_table[line_number].error.push_back("Operand Can't be greater than 0XFFFFF or less than 0 in format 4 !!");
					return false;
				}
				//bpe = 001
				objectCode.append("001");
				objectCode = bintohex(objectCode);
				objectCode.append(intToBinaryString(stoi(operand), 5));
			}else{
				if (stoi(operand) < 0 || stoi(operand) > 4095) {
					//4095 dec = fff hex
					listing_table[line_number].error.push_back("Operand Can't be greater than 0XFFF or less than 0 in format 3 !!");
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
	if(isOperandToTargetAddress(operand)){
		TA = address;
	}else{
		listing_table[line_number].error.push_back("Invalid operand !!");
		return false;
	}
	if (format == 4) {
		//TODO modification record
		objectCode.append("001");
		objectCode.append(bintohex(intToBinaryString(TA, 5)));
	}else{
		int disp = TA - (LOCCTR + format);
		if (-2048 <= disp && disp <= 2047){
			//PC-relative bpe = 010
			objectCode.append("010");
		}else if(base >= 0){
			//base available
			disp = TA - base;
			if (0 <= disp && disp <= 4095){
				//base relative bpe = 100
				objectCode.append("100");
			}else{
				listing_table[line_number].error.push_back("Can't Process the address!!");
				return false;
			}
		}
		objectCode.append(bintohex(intToBinaryString(disp, 3)));
	}
	if(objectCode.length() != 6 && format == 3){
		listing_table[line_number].error.push_back("Error in operand in format 3 !!");
		return false;
	}else if (objectCode.length() != 8 && format == 4){
		listing_table[line_number].error.push_back("Error in operand in format 4 !!");
		return false;
	}else {
		return true;
	}

}
	//first clear nix (Don't use regex in first stage)
	//test indexing
	//"\\,[Xx]$" nix = 111
	//delete \\,[Xx]
	//test not indexing
	//"^([#@])" nix = 010 or 100
	//delete [#@]
	//else normal operand nix = 110

	//second test if normal operand (* or int or label)
	//if matches "^(\\*|([-+]?\\d+)|(\\w+))$" normal operand
	//else try expression

	//third try match "^(\\*|\\w+)(\\+|\\-|\\/|\\*)(\\*|\\w+)$" then expression
	//else error

	//NOTEs : in expression user can't use negative int as first operand
	//if operand matches "^([-+]?\\d+)$" then is int external method
	//else operand is label

// if mnemonic == RSUB -> nixbpe = "110000" & disp = "000" f3 or "00000" f4

//direct means not relative
//format 4: direct addressing -> bpe = "001" & mRecord
//ni = "11" if simple addressing

//OLD SECTION
//	operand regex: "^(\\*)|([#@]?\\w+)|(\\w+\\,[Xx])$"
//  operand regex2: "^([#@])?((\\*)|(\\w+))(\\+|\\-|\\/|\\*)((\\*)|(\\w+))$"
//  operand regex2: "^((\\*)|(\\w+))(\\+|\\-|\\/|\\*)((\\*)|(\\w+))\\,[Xx]$"

//if \\w+ is integer -> direct addressing -> bpe = "000" & mRecord
//else if (in symbol_table) then calculate address
//else error

//calculate address
//IMPORTANT NOTE: PC = address of next instruction disp = TA - (current address + format)
//first try pc-relative calculate -2048 =< disp =< 2047
//else if base is available calculate 0 =< disp =< 4095
//else error
