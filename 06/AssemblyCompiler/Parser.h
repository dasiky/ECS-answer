#pragma once
#include <vector>
#include <string>
#include <sstream>

enum class commandTypeEnum
{ 
	A_COMMAND,
	C_COMMAND,
	L_COMMAND 
};

class Parser {
public:
	Parser(const std::string& filename);
	bool hasMoreCommands(); 
	void advance();
	void regress();
	commandTypeEnum commandType();
	std::string symbol();
	std::string dest();
	std::string comp();
	std::string jump();
private:
	std::vector<std::string> source{""};
	int index = 0;
};