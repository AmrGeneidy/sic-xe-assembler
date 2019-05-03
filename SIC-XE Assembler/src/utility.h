#ifndef UTILITY_H_
#define UTILITY_H_
#include<string>
#include<regex>
#include<fstream>


int binaryStringToInt(const std::string& binaryString);
string hextobin(const string &s);
string bintohex(const string &s);
bool exists_test0 (const std::string& name);

bool iequals(const std::string& str1, const std::string& str2);
std::string getUpperVersion(const std::string& x);

#endif /* UTILITY_H_ */
