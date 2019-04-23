#include<iostream>
#include<map>
#include<string>
#include<fstream>
#include<regex>
#include<vector>

#include"assemblerdata.h"
#include"utility.h"

using namespace std;

map<string, struct_opcode> opTable;
map<unsigned int, listing_line> listing_table;
map<string, unsigned int> symbol_table;

unsigned int starting_address;
unsigned int program_length;

unsigned int current_line_number;
unsigned int LOCCTR;
listing_line current_line;

void loadOpTable(string path) {
    string line;
    ifstream infile;
    infile.open(path);
    while (getline(infile, line)) {
        regex r("(\\w+)\\s+(\\w+)\\s+(\\w+)");
        smatch m;
        regex_search(line, m, r);
        opTable[m[1]].format = stoi(m[2]);
        opTable[m[1]].opcode = stoi(m[3], 0, 16);
        //TODO delete this line
        cout << m[1] << "      " << opTable[m[1]].format<< "      " << opTable[m[1]].opcode << endl ;
    }
    infile.close();
}

void build_listing_table(string path) {
    string line;
    ifstream infile;
    infile.open(path);
    int i = 0;
    while (getline(infile, line)) {
        regex rComment("(\\.)(.*)");
        smatch m;
        regex_search(line, m, rComment);
        if (m.size() > 0 && m.position(0) == 0) {
            listing_table[i].isAllComment = true;
            listing_table[i].comment = m[2];
            i++;
            continue;
        } else {
            regex rInstruction("(\\w?)(\\s*)(\\w?)(\\s*)(\\S?)(\\s*)(\\S?)");
            regex_search(line, m ,rInstruction);
            if(m.size() > 0) {
                listing_table[i].isAllComment = false;
                listing_table[i].label = m[1];
                listing_table[i].mnemonic = m[3];
                listing_table[i].operand = m[5];
                listing_table[i].comment = m[7];
            } else {
                listing_table[i].error.insert(listing_table[i].error.begin(), "Invalid Instruction");
            }
        }
        i++;
    }
    infile.close();
}

void pass1_Algorithm(string codePath) {
    //TODO load listing_table
    loadOpTable("optable.txt");
    current_line_number = 1;

    //skip the comments
    while (listing_table[current_line_number].isAllComment) {
        current_line_number++;
    }

    current_line = listing_table[current_line_number];
    //check start addressing
    if (iequals(current_line.mnemonic, "START")) {
        starting_address = stoi(current_line.operand, 0, 16);
        listing_table[current_line_number].address = starting_address;
        current_line = listing_table[++current_line_number];
    } else {
        starting_address = 0;
    }
    LOCCTR = starting_address;

    //reading code loop
    while (!iequals(current_line.mnemonic, "END")) {
        //process the line if not a comment
        if (!current_line.isAllComment) {
            //assign address to the line
            listing_table[current_line_number].address = LOCCTR;

            //process the label field
            if (!current_line.label.empty()) {
                if (symbol_table.find(current_line.label)
                    != symbol_table.end()) {
                    listing_table[current_line_number].error.push_back("symbol '"
                                                                       + current_line.label + "' is already defined");
                } else {
                    symbol_table[current_line.label] = LOCCTR;
                }
            }

            //process the mnemonic
            if (opTable.find(current_line.mnemonic) != opTable.end()) {
                //not directive
                if (opTable[current_line.mnemonic].format == 3 && current_line.isFormat4) {
                    LOCCTR += 4;
                } else if (opTable[current_line.mnemonic].format == 2 && current_line.isFormat4) {
                    listing_table[current_line_number].error.push_back(
                            "Can't use format 4 with mnemonic " + current_line.mnemonic);
                } else {
                    LOCCTR += opTable[current_line.mnemonic].format;
                }
            } else if (false /*TODO handle all the directives*/) {

            } else {
                listing_table[current_line_number].error.push_back("Invalid operation code");
            }
        }//end line process
        current_line = listing_table[++current_line_number];
    }
    program_length = LOCCTR - starting_address;

}

int main() {
//    build_listing_table("inputFile.txt");
    loadOpTable("optable.txt");
    return 0;
}
