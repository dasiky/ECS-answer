#pragma once
#include "Parser.h"
#include <string>
#include <fstream>

class CodeWrite {
public:
    void setFilename(const std::string& filename);
    void close();

    void writeArithmetic(const std::string& command);
    void writePushPop(Parser::CMD_TYPE type, const std::string& segment, int index, const std::string& filename);
    void writeInit();
    void writeLabel(const std::string& label);
    void writeGoto(const std::string& label);
    void writeIf(const std::string& label);
    void writeCall(const std::string& funcName, int numArgs);
    void writeReturn();
    void writeFunction(std::string funcName, int numLocals);
private:
    void wrAsm(const std::string& asmCMD);
    std::ofstream fs;
    std::string nowFunc;
    int compareCount = 0;
    int returnCount = 0;
};