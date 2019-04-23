/*
 * directivesHandler.cpp
 *
 *  Created on: Apr 23, 2019
 *      Author: OWNER
 */
#include"assemblerdata.h"

using namespace std;
map<string, struct_opcode> opTable;
map<unsigned int, listing_line> listing_table;
map<string, unsigned int> symbol_table;
unsigned int LOCCTR;
bool handleDirective(listing_line x){
	if(iequals(x.mnemonic, "byte")){

	}else if(iequals(x.mnemonic, "word")){

	}else if(iequals(x.mnemonic, "resw")){

	}else if(iequals(x.mnemonic, "resb")){

	}else if(iequals(x.mnemonic, "equ")){

	}else if(iequals(x.mnemonic, "org")){

	}else if(iequals(x.mnemonic, "base")){

	}else if(iequals(x.mnemonic, "nobase")){

	}else{
		return false;
	}
	return true;
}
bool handleByte(listing_line x){
	if(x.operand.size() < 3){
		return false;
	}
	if (x.operand[0] == 'x'||x.operand[0] == 'X') {
		if (x[1] == '1' &&) {

		}
	}if (x.operand[0] == 'c'||x.operand[0] == 'C') {

	}else
		return false;
}



