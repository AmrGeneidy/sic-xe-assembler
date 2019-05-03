#include<iostream>

#include"assemblerdata.h"
#include"utility.h"

using namespace std;

vector<int> tRecords;
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
	string objectCode = intToBinaryString(opTable[mnemonic].opcode);
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
	tRecords.push_back(binaryStringToInt(objectCode));
	return true;
}

//return false if there is an error in operand
bool instructionToObjectCode(listing_line x) {
	loadRegisters();
	string objectCode;
	string operand = getUpperVersion(x.operand);
	string mnemonic = getUpperVersion(x.mnemonic);
	unsigned int format = x.isFormat4 == true ? 4 : opTable[mnemonic].format;
	switch (format) {
		case 2:
			return handleFormat2(mnemonic, operand);
		case 3:
			break;
		case 4:
			//direct means not relative
			//format 4: direct addressing -> bpe = "001" & mRecord
			break;
	}
	return true;
}
//if mnemonic == RSUB -> nixbpe = "110000" & disp = "000" f3 or "00000" f4

//ni = "11" if simple addressing

//	operand regex: "^(\\*)|([#@]?\\w+)|(\\w+\\,[Xx])$"
//if \\w+ is integer -> direct addressing -> bpe = "000" & mRecord
//else if (in symbol_table) then calculate address
//else error

//calculate address
//IMPORTANT NOTE: PC = address of next instruction disp = TA - (current address + format)
//first try pc-relative calculate -2048 =< disp =< 2047
//else if base is available calculate 0 =< disp =< 4095
//else error
