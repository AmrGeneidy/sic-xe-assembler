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
bool handleByte(listing_line x);
bool handleWord(listing_line x);
bool handleRes(listing_line x, int increase_val);
bool handleOrg(listing_line x);
bool handleEqu(listing_line x);
bool handleBase(listing_line x);
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
		if (x.operand.empty() && x.label.empty()) {
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
	if (x.operand[0] == 'x' || x.operand[0] == 'X') {
		if (x.operand[1] == '\'' && x.operand[x.operand.size() - 1] == '\'') {
			try {
				stoi(x.operand, 0, 16);
			} catch (invalid_argument& e) {
				return false;
			}
			if (!x.label.empty()) {
				if (symbol_table.find(x.label) != symbol_table.end()) {
					return false;
				}
				symbol_table[x.label] = LOCCTR;
			}
			LOCCTR += (x.operand.size() - 2) / 2;
			return true;
		}
	} else if (x.operand[0] == 'c' || x.operand[0] == 'C') {
		if (x.operand[1] == '\'' && x.operand[x.operand.size() - 1] == '\'') {
			if (!x.label.empty()) {
				if (symbol_table.find(x.label) != symbol_table.end()) {
					return false;
				}
				symbol_table[x.label] = LOCCTR;
			}
			LOCCTR += x.operand.size() - 3;
			return true;
		}
	}
	return false;
}
bool handleWord(listing_line x) {
	//TODO handle operand 1,2,3 & #
	try {
		stoi(x.operand);
	} catch (invalid_argument& e) {
		return false;
	}
	if (!x.label.empty()) {
		if (!x.label.empty()) {
			if (symbol_table.find(x.label) != symbol_table.end()) {
				return false;
			}
			symbol_table[x.label] = LOCCTR;
		}
		symbol_table[x.label] = LOCCTR;
	}
	LOCCTR += 3;
	return true;
}
bool handleRes(listing_line x, int increase_val) {
	try {
		int z = stoi(x.operand);
		if (!x.label.empty()) {
			if (!x.label.empty()) {
				if (symbol_table.find(x.label) != symbol_table.end()) {
					return false;
				}
				symbol_table[x.label] = LOCCTR;
			}
			symbol_table[x.label] = LOCCTR;
		}
		LOCCTR += increase_val * z;
	} catch (invalid_argument& e) {
		return false;
	}
	return true;
}

bool handleOrg(listing_line x) {
	if (!x.label.empty()) {
		return false;
	} else if (symbol_table.find(x.operand) != symbol_table.end()) {
		LOCCTR = symbol_table[x.operand];
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
	if (x.label.empty()) {
		return false;
	} else if (symbol_table.find(x.operand) != symbol_table.end()) {
		symbol_table[x.label] = symbol_table[x.operand];
	} else if (iequals("*", x.operand)) {
		symbol_table[x.label] = LOCCTR;
	} else {
		try {
			int z = stoi(x.operand);
			symbol_table[x.label]= z;
		} catch (invalid_argument& e) {
			return false;
		}
	}
	return true;

}

bool handleBase(listing_line x){
	if (!x.label.empty()) {
		return false;
	}else if (symbol_table.find(x.operand) != symbol_table.end()) {

	}else if (iequals("*", x.operand)) {

	} else {
		try {
			int z = stoi(x.operand);
		} catch (invalid_argument& e) {
			return false;
		}
	}
	return true;
}
