#include "CodeWrite.h"
#include "Parser.h"
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2)
        return 0;

    std::vector<std::string> filenames;
    for (int i = 1; i < argc; i++) {
        filenames.emplace_back(argv[i]);
    }
    auto index = filenames[0].find_last_of("\\");
    std::string path = filenames[0].substr(0, index);
    index = path.find_last_of("\\");
    std::string asmfilename = path.substr(index + 1) + ".asm";
    asmfilename = path + "\\" + asmfilename;

    CodeWrite write;
    Parser parser(filenames);
    write.setFilename(asmfilename);
    write.writeInit();
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
            write.writePushPop(type, parser.arg1(), atoi(parser.arg2().c_str()), parser.filename());
            break;
        case Parser::CMD_TYPE::C_LABEL:
            write.writeLabel(parser.arg1());
            break;
        case Parser::CMD_TYPE::C_GOTO:
            write.writeGoto(parser.arg1());
            break;
        case Parser::CMD_TYPE::C_IF:
            write.writeIf(parser.arg1());
            break;
        case Parser::CMD_TYPE::C_CALL:
            write.writeCall(parser.arg1(), atoi(parser.arg2().c_str()));
            break;
        case Parser::CMD_TYPE::C_FUNCTION:
            write.writeFunction(parser.arg1(), atoi(parser.arg2().c_str()));
            break;
        case Parser::CMD_TYPE::C_RETURN:
            write.writeReturn();
            break;
        default:
            break;
        }
    }

    write.close();
    std::cout << "output: " << asmfilename << std::endl;
}