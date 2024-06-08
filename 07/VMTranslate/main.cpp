#include "CodeWrite.h"
#include "Parser.h"
#include <iostream>

int main(int argc, char** argv) {
	if (argc < 2)
		return 0;

	CodeWrite write;
	for (int i = 1; i < argc; i++) {
		std::string vmfilename = argv[i];
		Parser parser(vmfilename);
		auto index = vmfilename.find_last_of(".");
		std::string asmfilename = vmfilename.substr(0, index) + ".asm";
		write.setFilename(asmfilename);

		while (parser.hasMoreCommands()) {
			parser.advance();
			auto type = parser.commandType();
			switch (type)
			{
			case Parser::CMD_TYPE::C_ARITHMETIC:
				write.writeArithmetic(parser.cmd());
				break;
			case Parser::CMD_TYPE::C_PUSH:
			case Parser::CMD_TYPE::C_POP:
				write.writePushPop(type, parser.arg1(), atoi(parser.arg2().c_str()));
				break;
			default:
				break;
			}
		}

		write.close();
		std::cout << "output: " << asmfilename << std::endl;
	}
}