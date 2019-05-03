#ifndef UTILITY_H_
#define UTILITY_H_
#include<string>
#include<regex>
#include<fstream>
#include<algorithm>
#include<cmath>

bool is_number(const std::string& num);
std::string intToBinaryString(unsigned int &num);
int binaryStringToInt(const std::string& binaryString);
std::string hextobin(const std::string &s);
std::string bintohex(const std::string &s);
bool exists_test0 (const std::string& name);

bool iequals(const std::string& str1, const std::string& str2);
std::string getUpperVersion(const std::string& x);

#endif /* UTILITY_H_ */
