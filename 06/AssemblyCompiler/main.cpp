#include "Code.h"
#include "Parser.h"
#include <iostream>
#include <vector>

int main(int argc, char** argv) {
	if (argc != 2) return 0;
	Parser parser(argv[1]);

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
			ins += Code::comp(parser.comp());
			ins += Code::dest(parser.dest());
			ins += Code::jump(parser.jump());
			instructions.push_back(ins);
		}
	}

	for (auto ins : instructions)
		std::cout << ins << std::endl;
}