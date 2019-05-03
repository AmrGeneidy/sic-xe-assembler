#include<cmath>

#include"utility.h"

using namespace std;
//helping functions

int binaryStringToInt(const string &binaryString) {
	int value = 0;
	int indexCounter = 0;
	for (int i = binaryString.length() - 1; i >= 0; i--) {
		if (binaryString[i] == '1') {
			value += pow(2, indexCounter);
		}
		indexCounter++;
	}
	return value;
}

string hextobin(const string &s){
    string out;
    for(auto i: s){
        uint8_t n;
        if(i <= '9' and i >= '0')
            n = i - '0';
        else
            n = 10 + i - 'A';
        for(int8_t j = 3; j >= 0; --j)
            out.push_back((n & (1<<j))? '1':'0');
    }

    return out;
}

string bintohex(const string &s){
    string out;
    for(unsigned int i = 0; i < s.size(); i += 4){
        int8_t n = 0;
        for(unsigned int j = i; j < i + 4; ++j){
            n <<= 1;
            if(s[j] == '1')
                n |= 1;
        }

        if(n<=9)
            out.push_back('0' + n);
        else
            out.push_back('A' + n - 10);
    }

    return out;
}

bool exists_test0(const std::string& name) {
	ifstream f(name.c_str());
	return f.good();
}

string getUpperVersion(const string &x) {
	string temp;
	transform(x.begin(), x.end(), back_inserter(temp), ::toupper);
	return temp;
}

//iequals function check equality of two strings CASE-INSENSITIVE
struct iequal {
	bool operator()(int c1, int c2) const {
		return std::toupper(c1) == std::toupper(c2);
	}
};

bool iequals(const std::string& str1, const std::string& str2) {
	return (str1.size() == str2.size())
			&& std::equal(str1.begin(), str1.end(), str2.begin(), iequal());
}
//end iequals
