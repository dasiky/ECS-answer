#pragma once
#include "Parser.h"
#include <string>
#include <fstream>

class CodeWrite {
public:
	void setFilename(const std::string& filename);
	void writeArithmetic(const std::string& command);
	void writePushPop(Parser::CMD_TYPE type, const std::string& segment, int index);
	void close();
private:
	void wrAsm(const std::string& asmCMD);
	void initialize();
	std::ofstream fs;
	int compareCount = 0;
};