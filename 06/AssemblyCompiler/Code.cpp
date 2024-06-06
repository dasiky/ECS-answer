#include "Code.h"
#include <map>

std::string Code::dest(const std::string& mnemonic) {
	std::string res(3, '0');
	if (mnemonic.find('M') != std::string::npos)
		res[2] = '1';
	if (mnemonic.find('D') != std::string::npos)
		res[1] = '1';
	if (mnemonic.find('A') != std::string::npos)
		res[0] = '1';
	return res;
}

std::string Code::comp(const std::string& mnemonic) {
	static std::map<std::string, std::string> mp{
		{"0", "0101010"},
		{"1", "0111111"},
		{"-1", "0111010"},
		{"D", "0001100"},
		{"A", "0110000"},
		{"!D", "0001101"},
		{"!A", "0110001"},
		{"-D", "0001111"},
		{"-A", "0110011"},
		{"D+1", "0011111"},
		{"A+1", "0110111"},
		{"D-1", "0001110"},
		{"A-1", "0110010"},
		{"D+A", "0000010"},
		{"D-A", "0010011"},
		{"A-D", "0000111"},
		{"D&A", "0000000"},
		{"D|A", "0010101"},

		{"M", "1110000"},
		{"!M", "1110001"},
		{"-M", "1110011"},
		{"M+1", "1110111"},
		{"M-1", "1110010"},
		{"D+M", "1000010"},
		{"D-M", "1010011"},
		{"M-D", "1000111"},
		{"D&M", "1000000"},
		{"D|M", "1010101"}
	};

	auto iter = mp.find(mnemonic);
	if (iter == mp.end())
		return "";
	return iter->second;
}

std::string Code::jump(const std::string& mnemonic) {
	static std::map<std::string, std::string> mp{
		{"", "000"},
		{"JGT", "001"},
		{"JEQ", "010"},
		{"JGE", "011"},
		{"JLT", "100"},
		{"JNE", "101"},
		{"JLE", "110"},
		{"JMP", "111"},
	};

	return mp[mnemonic];
}