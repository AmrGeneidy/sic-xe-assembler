#include<map>
#include<string>
#include<vector>
#include<regex>


using namespace std;

#ifndef ASSEMBLERDATA_H_
#define ASSEMBLERDATA_H_


struct struct_opcode {
	string opcode;
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
	int address;
	char type;
	symbol_struct() : address(0), type('R') {}

};
extern map<string, struct_opcode> opTable;
extern map<unsigned int, listing_line> listing_table;
//key: label name ,  value: address
extern map<string, symbol_struct> symbol_table;
extern vector<string> tRecords;

extern unsigned int starting_address;
extern unsigned int program_length;
extern unsigned int LOCCTR;
//if base is negative then NOBASE
extern int base;
extern int address;

void build_listing_table(string path);
bool handleDirective(listing_line x);
bool isDirective(string x);
bool isRelocatable(string exp);
bool isAbsluteExp(string exp);

#endif /* ASSEMBLERDATA_H_ */
