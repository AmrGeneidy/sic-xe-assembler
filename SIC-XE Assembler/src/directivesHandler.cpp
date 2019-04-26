/*
 * directivesHandler.cpp
 *
 *  Created on: Apr 23, 2019
 *      Author: OWNER
 */
#include <sstream>
#include"assemblerdata.h"
#include"utility.h"

using namespace std;
int address;
int arrayLength;
bool isArray(string exp) {
	smatch m;
	regex r("-?\\d+(\\,-?\\d+)+");
	if (regex_match(exp, m, r)) {
		 stringstream ss( exp );
		    vector<string> result;

		    while( ss.good() )
		    {
		        string substr;
		        getline( ss, substr, ',' );
		        result.push_back( substr );
		    }
		    arrayLength = result.size();
		return true;
	}
	return false;
}
bool isInt(string op) {
	try {
		stoi(op);
		return true;
	} catch (invalid_argument& e) {
		return false;
	}
}
bool isRelative(string symbol) {
	return symbol_table.find(symbol) != symbol_table.end()
			&& symbol_table[symbol].type == 'R';
}
bool isAbslute(string symbol) {
	return symbol_table.find(symbol) != symbol_table.end()
			&& symbol_table[symbol].type == 'A';
}
bool isRelocatable(string exp) {
	smatch m;
	regex r("(\\w+)([\\+\\-\\*\\/])(\\w+)");
	if (regex_match(exp, m, r)) {
		string operand1 = getUpperVersion(m[1]);
		string operat = getUpperVersion(m[2]);
		string operand2 = getUpperVersion(m[3]);
		if (iequals("+", operat)) {
			if ((isRelative(operand1) && isAbslute(operand2))
					|| (isRelative(operand2) && isAbslute(operand1))) {
				address = symbol_table[operand1].address
						+ symbol_table[operand2].address;
				return address >= 0;
			} else if (isRelative(operand1) && isInt(operand2)) {
				address = symbol_table[operand1].address + stoi(operand2);
				return address >= 0;
			} else if (isRelative(operand2) && isInt(operand1)) {
				address = symbol_table[operand2].address + stoi(operand1);
				return address >= 0;
			}
		} else if (iequals("-", operat)) {
			if (isRelative(operand1) && isInt(operand2)) {
				address = symbol_table[operand1].address - stoi(operand2);
				return address >= 0;
			}
		}
	}
	return false;
}
bool isAbsluteExp(string exp) {
	smatch m;
	regex r("(\\w+)([\\+\\-\\*\\/])(\\w+)");
	if (regex_match(exp, m, r)) {
		string operand1 = getUpperVersion(m[1]);
		string operat = getUpperVersion(m[2]);
		string operand2 = getUpperVersion(m[3]);
		if (iequals("+", operat)) {
			if (isAbslute(operand1) && isAbslute(operand2)) {
				address = symbol_table[operand1].address
						+ symbol_table[operand2].address;
				return true;
			} else if (isAbslute(operand1) && isInt(operand2)) {
				address = symbol_table[operand1].address + stoi(operand2);
				return true;
			} else if (isAbslute(operand2) && isInt(operand1)) {
				address = symbol_table[operand2].address + stoi(operand1);
				return true;
			} else if (isInt(operand1) && isInt(operand2)) {
				address = stoi(operand1) + stoi(operand2);
				return true;
			} else {
				return false;
			}
		} else if (iequals("-", operat)) {
			if (isAbslute(operand1) && isAbslute(operand2)) {
				address = symbol_table[operand1].address
						- symbol_table[operand2].address;
				return true;
			} else if (isAbslute(operand1) && isInt(operand2)) {
				address = symbol_table[operand1].address - stoi(operand2);
				return true;
			} else if (isAbslute(operand2) && isInt(operand1)) {
				address = -symbol_table[operand2].address + stoi(operand1);
				return true;
			} else if (isInt(operand1) && isInt(operand2)) {
				address = stoi(operand1) - stoi(operand2);
				return true;
			} else if (isRelative(operand1) && isRelative(operand2)) {
				address = symbol_table[operand1].address
						- symbol_table[operand2].address;
				;
				return true;
			} else {
				return false;
			}
		} else if (iequals("*", operat)) {
			if (isAbslute(operand1) && isAbslute(operand2)) {
				address = symbol_table[operand1].address
						* symbol_table[operand2].address;
				return true;
			} else if (isAbslute(operand1) && isInt(operand2)) {
				address = symbol_table[operand1].address * stoi(operand2);
				return true;
			} else if (isAbslute(operand2) && isInt(operand1)) {
				address = symbol_table[operand2].address * stoi(operand1);
				return true;
			} else if (isInt(operand1) && isInt(operand2)) {
				address = stoi(operand1) * stoi(operand2);
				return true;
			} else {
				return false;
			}
		} else if (iequals("/", operat)) {
			if (isAbslute(operand1) && isAbslute(operand2)) {
				address = symbol_table[operand1].address
						/ symbol_table[operand2].address;
				return true;
			} else if (isAbslute(operand1) && isInt(operand2)) {
				address = symbol_table[operand1].address / stoi(operand2);
				return true;
			} else if (isAbslute(operand2) && isInt(operand1)) {
				address = stoi(operand1) / symbol_table[operand2].address;
				return true;
			} else if (isInt(operand1) && isInt(operand2)) {
				address = stoi(operand1) / stoi(operand2);
				return true;
			} else {
				return false;
			}
		} else {
			return false;
		}
	} else {
		return false;
	}
}
bool handleByte(listing_line x);
bool handleWord(listing_line x);
bool handleRes(listing_line x, int increase_val);
bool handleOrg(listing_line x);
bool handleEqu(listing_line x);
bool handleBase(listing_line x);

bool isDirective(string x) {
	if (iequals(x, "START") || iequals(x, "END") || iequals(x, "BYTE")
			|| iequals(x, "WORD") || iequals(x, "RESW") || iequals(x, "RESB")
			|| iequals(x, "EQU") || iequals(x, "ORG") || iequals(x, "BASE")
			|| iequals(x, "NOBASE")) {
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
	if ((x.operand[0] == 'x' || x.operand[0] == 'X')
			&& x.operand.size() % 2 == 1) {
		if (x.operand[1] == '\'' && x.operand[x.operand.size() - 1] == '\'') {
			try {
				stoi(x.operand.substr(2, x.operand.size() - 3), 0, 16);
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
	if (isArray(x.operand)) {
		LOCCTR += 3 * arrayLength;
	} else
		try {
			stoi(x.operand);
			LOCCTR += 3;
		} catch (invalid_argument& e) {

			return false;
		}

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
		LOCCTR = symbol_table[operand].address;
		return true;
	} else if (operand[0] == '*' && operand.size() == 1) {
		return true;
	} else if (isRelocatable(operand)) {
		LOCCTR = address;
		return true;
	}
	try {
		int z = stoi(x.operand);
		LOCCTR = z;
		return true;
	} catch (invalid_argument& e) {
		return false;
	}

}

bool handleEqu(listing_line x) {
	string label = getUpperVersion(x.label);
	string operand = getUpperVersion(x.operand);
	if (x.label.empty() || operand.empty()) {
		return false;
	} else if (symbol_table.find(operand) != symbol_table.end()) {
		symbol_table[label].address = symbol_table[operand].address;
	} else if (iequals("*", operand)) {
		symbol_table[label].address = LOCCTR;
	} else {

		if (isRelocatable(operand)) {
			symbol_table[label].address = address;
			return true;
		} else if (isAbslute(operand)) {
			symbol_table[label].address = address;
			symbol_table[label].type = 'A';
			return true;
		}
		try {
			int z = stoi(x.operand);
			symbol_table[label].address = z;
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
			if (isRelocatable(operand)) {
				return true;
			}
			stoi(x.operand);
		} catch (invalid_argument& e) {
			return false;
		}
	}
	return true;
}
