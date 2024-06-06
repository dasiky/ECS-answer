#include "Code.h"
#include "Parser.h"
#include <iostream>
#include <vector>
#include <fstream>

int main(int argc, char** argv) {
	if (argc != 2) return 0;
	std::string inputFile = argv[1];
	Parser parser(inputFile);
	std::vector<std::string> instructions;
	while (parser.hasMoreCommands()) {
		parser.advance();

		auto cmdType = parser.commandType();
		if (cmdType == commandTypeEnum::A_COMMAND) {
			std::string symbol = parser.symbol();
			std::bitset<15> bits(atoi(symbol.c_str()));
			instructions.push_back("0" + bits.to_string());
		}
		else if (cmdType == commandTypeEnum::L_COMMAND) {

		}
		else {
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
	std::cout << "output: " << outputFile << std::endl;
	std::ofstream of(outputFile);
	for (auto ins : instructions) {
		std::cout << ins << std::endl;
		of << ins << std::endl;
	}
	of.close();
}