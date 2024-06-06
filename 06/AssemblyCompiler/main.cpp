#include "Code.h"
#include "Parser.h"
#include "SymbolTable.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <set>

bool isNum(const std::string& s)
{
	for (auto c : s)
		if (!isdigit(c))
			return false;
	return true;
}

int main(int argc, char** argv) {
	if (argc != 2) return 0;
	std::string inputFile = argv[1];
	Parser parser(inputFile);

	// 建立符号表
	SymbolTable symbolTable;
	std::vector<std::string> varSymbols;
	int countActualCMD = 0;
	while (parser.hasMoreCommands()) {
		parser.advance();
		auto cmdType = parser.commandType();
		std::string symbol = parser.symbol();
		if (cmdType == commandTypeEnum::A_COMMAND) {
			if (!isNum(symbol))
				varSymbols.push_back(symbol);
		}
		else if (cmdType == commandTypeEnum::L_COMMAND)
			symbolTable.addEntry(symbol, countActualCMD);

		if (cmdType != commandTypeEnum::L_COMMAND)
			countActualCMD++;
	}
	int nextRAMUse = 16;
	for (auto symbol : varSymbols) {
		if (symbolTable.contains(symbol)) 
			continue;

		symbolTable.addEntry(symbol, nextRAMUse++);
	}

	parser.regress();
	// 生成机器码
	std::vector<std::string> instructions;
	while (parser.hasMoreCommands()) {
		parser.advance();

		auto cmdType = parser.commandType();
		if (cmdType == commandTypeEnum::A_COMMAND) {
			std::string symbol = parser.symbol();
			std::bitset<15> bits;
			if (isNum(symbol))
				bits = atoi(symbol.c_str());
			else
				bits = symbolTable.GetAddress(symbol);
			instructions.push_back("0" + bits.to_string());
		}
		else if (cmdType == commandTypeEnum::C_COMMAND){
			std::string ins("111");
			std::string mnemonic;
			mnemonic = parser.comp();
			ins += Code::comp(mnemonic);
			mnemonic = parser.dest();
			ins += Code::dest(mnemonic);
			mnemonic = parser.jump();
			ins += Code::jump(mnemonic);

			instructions.push_back(ins);
		}
	}

	auto index = inputFile.find_last_of('.');
	std::string outputFile = inputFile.substr(0, index);
	outputFile += ".hack";
	std::ofstream of(outputFile);
	for (auto ins : instructions) {
		std::cout << ins << std::endl;
		of << ins << std::endl;
	}
	of.close();
	std::cout << "output: " << outputFile << std::endl;
}