#include<map>
#include<string>
#include<vector>

using namespace std;

#ifndef ASSEMBLERDATA_H_
#define ASSEMBLERDATA_H_


struct struct_opcode {
	unsigned int opcode;
	unsigned int format;
};

struct listing_line {
	bool isAllComment;
	unsigned int address;
	bool isFormat4;
	string label;
	string mnemonic;
	string operand;
	string comment;
	vector<string> error;
};

struct symbol_struct{
	unsigned int address;
	char type;
	symbol_struct() : address(0), type('R') {}

};
extern map<string, struct_opcode> opTable;
extern map<unsigned int, listing_line> listing_table;
//key: label name ,  value: address
extern map<string, symbol_struct> symbol_table;

extern unsigned int starting_address;
extern unsigned int program_length;
extern unsigned int LOCCTR;

void build_listing_table(string path);
bool handleDirective(listing_line x);
bool isDirective(string x);

#endif /* ASSEMBLERDATA_H_ */
