#include "Parser.h"
#include <fstream>
#include <iostream>

Parser::Parser(const std::string& filename) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "can't open file." << filename << std::endl;
		return ;
	}

	std::string line;
	while (std::getline(file, line)) {
		if (line != "" && line.substr(0, 2) != "//")
			source.push_back(line);
	};
	file.close();
}

bool Parser::hasMoreCommands() {
	return index < source.size() - 1;
}

void Parser::advance() {
	index++;
}

commandTypeEnum Parser::commandType() {
	auto& currentCommand = source[index];
	if (currentCommand[0] == '@')
		return commandTypeEnum::A_COMMAND;
	if (currentCommand[0] == '(')
		return commandTypeEnum::L_COMMAND;
	return commandTypeEnum::C_COMMAND;
}

std::string Parser::symbol() {
	auto& currentCommand = source[index];
	switch (commandType())
	{
	case commandTypeEnum::A_COMMAND:
		return currentCommand.substr(1);
	case commandTypeEnum::L_COMMAND:
		return currentCommand.substr(1, currentCommand.size() - 2);
	default:
		return "";
	}
}

std::string Parser::dest() {
	auto& currentCommand = source[index];
	auto index = currentCommand.find('=');
	if (index == std::string::npos)
		return "";
	return currentCommand.substr(0, index);
}

std::string Parser::comp() {
	auto& currentCommand = source[index];
	auto eq = currentCommand.find('=');
	auto semicolon = currentCommand.find(';');

	if (eq == std::string::npos)
		eq = -1;
	if (semicolon == std::string::npos)
		semicolon = currentCommand.size();

	return currentCommand.substr(eq + 1, semicolon - eq - 1);
}

std::string Parser::jump() {
	auto& currentCommand = source[index];
	auto index = currentCommand.find(';');

	if (index == std::string::npos)
		return "";

	return currentCommand.substr(index + 1);
}
