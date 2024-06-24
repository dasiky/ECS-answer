#pragma once
#include <string>
#include <fstream>

enum class SEGMENT {
	CONST,
	ARG,
	LOCAL,
	STATIC,
	THIS,
	THAT,
	POINTER,
	TEMP,
};

enum class ARITH_CMD {
	ADD,
	SUB,
	NEG,
	EQ,
	GT,
	LT,
	AND,
	OR,
	NOT
};

class VMWriter {
public:
	VMWriter(const std::string& outPath);
	void writePush(SEGMENT seg, int index);
	void writePop(SEGMENT seg, int index);
	void writeArithmetic(ARITH_CMD cmd);
	void writeLabel(const std::string& label);
	void writeGoto(const std::string& label);
	void writeIf(const std::string& label);
	void writeCall(const std::string& name, int nArgs);
	void writeFunction(const std::string& name, int nArgs);
	void writeReturn();
	void close();
private:
	std::ofstream ofs;
};