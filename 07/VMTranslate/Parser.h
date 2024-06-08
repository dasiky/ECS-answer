#pragma once
#include <vector>
#include <string>

class Parser {
public:
	enum class CMD_TYPE {
		C_ARITHMETIC,
		C_PUSH,
		C_POP,
		C_LABEL,
		C_GOTO,
		C_IF,
		C_FUNCTION,
		C_RETURN,
		C_CALL
	};
public:
	Parser(const std::string& filename);
	bool hasMoreCommands();
	void advance();
	CMD_TYPE commandType();
	std::string cmd();
	std::string arg1();
	std::string arg2();
private:
	std::vector<std::vector<std::string>> source{{""}};
	size_t index = 0;
};