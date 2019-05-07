#include<iostream>

#include"assemblerdata.h"
#include"utility.h"

using namespace std;

int base = -1;
vector<string> tRecords;
map<string, string> registers;


void loadRegisters(){
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

bool handleFormat2(string mnemonic, string operand){
	string objectCode = hextobin(opTable[mnemonic].opcode);
	smatch m;
	regex r("^(\\w+)(\\,(\\w+))?$");
	regex_search(operand, m, r);
	if (m.size() > 0 && registers.find(m[1].str()) != registers.end()) {
		//register 1 exist
		objectCode.append(registers[m[1].str()]);
		if (m[3].str().empty()) {
			//no register 2
			objectCode.append("0000");
		}else{
			if(registers.find(m[3].str()) != registers.end()){
				//register 2 exist
				objectCode.append(registers[m[3].str()]);
			}else{
				return false;
			}
		}
	}else {
		return false;
	}
	tRecords.push_back(bintohex(objectCode));
	return true;
}

bool handleOneOperand(string mnemonic, string operand){
	string nixbpe;
	string objectCode = hextobin(opTable[mnemonic].opcode).substr(0, 6);
	smatch m;
	regex r("^([#@]?(([-+]?\\w+)|(\\*)))|(((\\*)|([-+]?\\w+))\\,[Xx])$");
	regex_search(operand, m, r);
	if (m.size() > 0){
		//not indexing
		if(!m[1].str().empty()){
			switch (operand[0]){
				case '#':
					nixbpe = "010";
					operand = operand.substr(1, operand.size() - 1);
					break;
				case '@':
					nixbpe = "100";
					operand = operand.substr(1, operand.size() - 1);
					break;
				default :
					nixbpe = "110";
			}
			// is int matches ^[-+]?\\d+$

			//handle bpe & address
		}else if (!m[5].str().empty()){
			nixbpe = "111";
			operand = operand.substr(0, operand.size() - 2);
			//handle bpe & address
		}else {
			return false;
		}
		return true;
	}else{
		return false;
	}
}

bool handleExpressionOperand(string mnemonic, string operand){
	string objectCode = hextobin(opTable[mnemonic].opcode).substr(0, 6);
}

//return false if there is an error in operand
bool instructionToObjectCode(listing_line x) {
	loadRegisters();
	string objectCode;
	string operand = getUpperVersion(x.operand);
	string mnemonic = getUpperVersion(x.mnemonic);
	unsigned int format = x.isFormat4 == true ? 4 : opTable[mnemonic].format;
	if (format == 2) {
		return handleFormat2(mnemonic, operand);
	}
	//handle special case RSUB has no operand
	if (mnemonic == "RSUB") {
		objectCode = bintohex(objectCode.append("110000"));
		objectCode.append("000");
		if(format == 4){
			objectCode.append("00");
		}
		return true;
	}
	objectCode = hextobin(opTable[mnemonic].opcode).substr(0, 6);
	string last2chars = operand[operand.size()-1] + operand[operand.size()-2];
	if(iequals(last2chars, ",X")){
		//indexing nix = 111
		objectCode.append("111");
		operand = operand.substr(0, operand.size()-2);
	}else if(operand[0] == '#' || operand[0] == '@'){
		//immediate or indirect addressing
		if(operand[0] == '#'){
			//immediate nix = 010
			objectCode.append("010");
		}else{
			//indirect nix = 100
			objectCode.append("100");
		}
		operand = operand.substr(1, operand.size()-1);
	}else{
		//simple addressing with no indexing nix = 110
		objectCode.append("110");
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


	//this methods will have a lot of arguments, solution -> make a struct
	if (handleOneOperand(mnemonic, operand) || handleExpressionOperand(mnemonic, operand)) {
		return true;
	}else{
		return false;
	}
}
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
