#include<iostream>
#include<map>
#include<string>
#include<fstream>
#include<regex>

#include"assemblerdata.h"
#include"utility.h"

bool is_mnemonicOrDirective(string x) {
	string temp = getUpperVersion(x);
	if ((opTable.find(temp) != opTable.end()) || isDirective(temp)) {
		return true;
	} else if (temp.at(0) == '+'
			&& opTable.find(temp.substr(1, temp.size() - 1)) != opTable.end()) {
		return true;
	}
	return false;
}

bool parse_label(string x, int i) {
	smatch m;
	regex r("^([A-Za-z]\\w*)$");
	regex_search(x, m, r);
	if (m.size() > 0) {
		listing_table[i].label = m[1].str();
		return true;
	}
	return false;
}

bool parse_mnemonic(string x, int i) {
	smatch m;
	regex r("^(\\+?(\\w+))$");
	regex_search(x, m, r);
	if (m.size() > 0) {
		if (m[1].str().at(0) == '+') {
			listing_table[i].isFormat4 = true;
		}
		listing_table[i].mnemonic = m[2].str();
		return true;
	}
	return false;
}

bool parse_operand(string x, int i) {
	smatch m;
	regex r("^(\\*|([#@]?\\w+)|(\\w+(\\'|\\,|\\+|\\-|\\*|\\/)\\w+)(\\')?)$");
	regex_search(x, m, r);
	if (m.size() > 0) {
		listing_table[i].operand = m[1].str();
		return true;
	}
	regex r2("^(-?\\d(\\,-?\\d)*)$");
	regex_search(x, m, r2);
	if (m.size() > 0) {
		listing_table[i].operand = m[1].str();
		return true;
	}
	return false;
}

//3 cases
bool parse_instruction(string x[], int i) {
	bool noError = true;
	//case 1 mnemonic only
	if (x[0].empty() && !x[1].empty() && x[2].empty()) {
		noError = parse_mnemonic(x[1], i);
	} //case 2 label, mnemonic and operand exists
	else if (!x[0].empty() && !x[1].empty() && !x[2].empty()) {
		noError = parse_label(x[0], i) & parse_mnemonic(x[1], i)
				& parse_operand(x[2], i);
	} //case 3 (label and mnemonic) OR (mnemonic and operand)
	else if (!x[0].empty() && !x[1].empty() && x[2].empty()) {
		//label and mnemonic
		if (is_mnemonicOrDirective(x[1])) {
			noError = parse_label(x[0], i) & parse_mnemonic(x[1], i);
		} //mnemonic and operand
		else if (is_mnemonicOrDirective(x[0])) {
			noError = parse_mnemonic(x[0], i) & parse_operand(x[1], i);
		} else {
			return false;
		}
	} else {
		return false;
	}
	return noError;

}

void build_listing_table(string path) {
	string line;
	ifstream infile;
	infile.open(path);
	int i = 0;
	while (getline(infile, line)) {
		regex rComment("^(\\.)(.*)");
		smatch m;
		regex_search(line, m, rComment);
		if (m.size() > 0 && m.position(0) == 0) {
			listing_table[i].isAllComment = true;
			listing_table[i].comment = m[0].str();
			i++;
			continue;
		} else {
			//ensure that label, mnemonic and operand don't start with "." (comment filter)
			regex rInstruction(
					"^\\s*(([^\\.]\\S*)\\s+)?([^\\.]\\S*)\\s*(\\s+([^\\.]\\S*))?\\s*(\\s+(\\..*))?$");
			regex_search(line, m, rInstruction);
			if (m.size() > 0) {
				listing_table[i].comment = m[7].str();
				string instruction[] = { m[2].str(), m[3].str(), m[5].str() };
				if (!parse_instruction(instruction, i)) {
					listing_table[i].error.insert(
							listing_table[i].error.begin(),
							"Invalid Instruction");
				}
			} else {
				listing_table[i].error.insert(listing_table[i].error.begin(),
						"Invalid Instruction");
			}
		}
		i++;
	}
	infile.close();
}
