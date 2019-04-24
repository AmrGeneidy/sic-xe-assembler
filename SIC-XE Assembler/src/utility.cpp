#include"utility.h"

//helping functions

//iequals function check equality of two strings CASE-INSENSITIVE
struct iequal {
	bool operator()(int c1, int c2) const {
		return std::toupper(c1) == std::toupper(c2);
	}
};

bool iequals(const std::string& str1, const std::string& str2) {
	return (str1.size()==str2.size())&&std::equal(str1.begin(), str1.end(), str2.begin(), iequal());
}
//end iequals
