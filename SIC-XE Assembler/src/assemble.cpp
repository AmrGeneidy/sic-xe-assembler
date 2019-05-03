#include<iostream>

#include"assemblerdata.h"
#include"utility.h"

using namespace std;

vector<int> tRecords;



void handleFormat2(string mnemonic, string operand){
	string objectCode = intToBinaryString(opTable[mnemonic].opcode);
}


void instructionToObjectCode(listing_line x) {
	string objectCode;
	string operand = getUpperVersion(x.operand);
	string mnemonic = getUpperVersion(x.mnemonic);
	unsigned int format = x.isFormat4 == true ? 4 : opTable[mnemonic].format;
	switch (format) {
		case 2:
			handleFormat2(mnemonic, operand);
			return;
		case 3:
			break;
		case 4:
			break;
	}
}

