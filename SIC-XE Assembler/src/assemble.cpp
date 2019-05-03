#include<iostream>

#include"assemblerdata.h"
#include"utility.h"

using namespace std;


vector<int> tRecords;

void instructionToObjectCode(listing_line x){
	string objectCode;
	string mnemonic = getUpperVersion(x.mnemonic);
	unsigned int format = x.isFormat4 == true ? 4 : opTable[mnemonic].format;
}
