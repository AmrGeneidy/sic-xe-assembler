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


extern map<string, struct_opcode> opTable;
extern map<unsigned int, listing_line> listing_table;
//key: label name ,  value: address
extern map<string, unsigned int> symbol_table;

extern unsigned int starting_address;
extern unsigned int program_length;
extern unsigned int LOCCTR;

//extern bool handleDirective(listing_line x);



#endif /* ASSEMBLERDATA_H_ */
