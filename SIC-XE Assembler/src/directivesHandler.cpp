/*
 * directivesHandler.cpp
 *
 *  Created on: Apr 23, 2019
 *      Author: OWNER
 */
#include"assemblerdata.h"
#include"utility.h"

using namespace std;

bool handleByte(listing_line x);
bool handleWord(listing_line x);
bool handleRes(listing_line x, int increase_val);
bool handleOrg(listing_line x);
bool handleEqu(listing_line x);
bool handleBase(listing_line x);

bool isDirective(string x){
	if (iequals(x, "START")||iequals(x, "END")||iequals(x, "BYTE")||iequals(x, "WORD")||iequals(x, "RESW")||iequals(x, "RESB")||iequals(x, "EQU")||iequals(x, "ORG")||iequals(x, "BASE")||iequals(x, "NOBASE")) {
		return true;
	}
	return false;
}

bool handleDirective(listing_line x) {
	if (iequals(x.mnemonic, "byte")) {
		return handleByte(x);
	} else if (iequals(x.mnemonic, "word")) {
		return handleWord(x);
	} else if (iequals(x.mnemonic, "resw")) {
		return handleRes(x, 3);
	} else if (iequals(x.mnemonic, "resb")) {
		return handleRes(x, 1);
	} else if (iequals(x.mnemonic, "equ")) {
		return handleEqu(x);
	} else if (iequals(x.mnemonic, "org")) {
		return handleOrg(x);
	} else if (iequals(x.mnemonic, "base")) {
		return handleBase(x);
	} else if (iequals(x.mnemonic, "nobase")) {
		if (!(x.operand.empty() && x.label.empty())) {
			return false;
		}
	} else {
		return false;
	}
	return true;
}
bool handleByte(listing_line x) {
	if (x.operand.size() < 3) {
		return false;
	}
	//TODO maxLength
	if ((x.operand[0] == 'x' || x.operand[0] == 'X')&& x.operand.size()%2 == 1) {
		if (x.operand[1] == '\'' && x.operand[x.operand.size() - 1] == '\'') {
			try {
				stoi(x.operand.substr(2,x.operand.size() - 3), 0, 16);
			} catch (invalid_argument& e) {
				return false;
			}
			LOCCTR += (x.operand.size() - 3) / 2;
			return true;
		}
	} else if (x.operand[0] == 'c' || x.operand[0] == 'C') {
		if (x.operand[1] == '\'' && x.operand[x.operand.size() - 1] == '\'') {
			LOCCTR += x.operand.size() - 3;
			return true;
		}
	}
	return false;
}
bool handleWord(listing_line x) {
	//TODO handle operand 1,2,3
	try {
		stoi(x.operand);
	} catch (invalid_argument& e) {
		return false;
	}
	LOCCTR += 3;
	return true;
}
bool handleRes(listing_line x, int increase_val) {
	try {
		int z = stoi(x.operand);
		LOCCTR += increase_val * z;
	} catch (invalid_argument& e) {
		return false;
	}
	return true;
}

bool handleOrg(listing_line x) {
	string operand = getUpperVersion(x.operand);
	if (!x.label.empty()) {
		return false;
	} else if (symbol_table.find(operand) != symbol_table.end()) {
		LOCCTR = symbol_table[operand];
	} else {
		try {
			int z = stoi(x.operand);
			LOCCTR = z;
		} catch (invalid_argument& e) {
			return false;
		}
	}
	return true;
}

bool handleEqu(listing_line x) {
	string label = getUpperVersion(x.label);
	string operand = getUpperVersion(x.operand);
	if (x.label.empty()) {
		return false;
	} else if (symbol_table.find(operand) != symbol_table.end()) {
		symbol_table[label] = symbol_table[operand];
	} else if (iequals("*", operand)) {
		symbol_table[label] = LOCCTR;
	} else {
		try {
			int z = stoi(x.operand);
			symbol_table[label] = z;
		} catch (invalid_argument& e) {
			return false;
		}
	}
	return true;

}

bool handleBase(listing_line x) {
	string operand = getUpperVersion(x.operand);
	if (!x.label.empty()) {
		return false;
	} else if (symbol_table.find(operand) != symbol_table.end()) {

	} else if (iequals("*", operand)) {

	} else {
		try {
			int z = stoi(x.operand);
		} catch (invalid_argument& e) {
			return false;
		}
	}
	return true;
}
