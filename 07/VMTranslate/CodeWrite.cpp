#include "CodeWrite.h"
#include <iostream>
#include <unordered_map>

void CodeWrite::setFilename(const std::string& filename) {
	initialize();
	fs = std::ofstream(filename);
	if (!fs.is_open()) {
		std::cerr << "can't open file." << filename << std::endl;
		return;
	}
}

void CodeWrite::writeArithmetic(const std::string& command) {
	wrAsm("@SP");
	wrAsm("A=M");
	wrAsm("A=A-1");

	// 将使用一个参数的运算和使用两个参数的运算分开处理
	if (command == "neg" || command == "not") {
		if (command == "neg")
			wrAsm("M=-M");
		else
			wrAsm("M=!M");
	}
	else {
		wrAsm("D=M");
		wrAsm("A=A-1");

		if (command == "add")
			wrAsm("M=D+M");
		else if (command == "sub")
			wrAsm("M=M-D");
		else if (command == "and")
			wrAsm("M=D&M");
		else if (command == "or")
			wrAsm("M=D|M");
		else { // 三种比较，每次比较使用jmp跳转来赋值
			std::string trueSymbol = "_True_" + std::to_string(compareCount);
			std::string endSymbol = "_CmpEnd_" + std::to_string(compareCount);

			wrAsm("D=M-D");
			wrAsm("@" + trueSymbol);
			if (command == "eq")
				wrAsm("D;JEQ");
			if (command == "gt")
				wrAsm("D;JGT");
			if (command == "lt")
				wrAsm("D;JLT");
			wrAsm("D=0");
			wrAsm("@" + endSymbol);
			wrAsm(";JMP");
			wrAsm("(" + trueSymbol + ")");
			wrAsm("D=-1");
			wrAsm("(" + endSymbol + ")");

			wrAsm("@SP");
			wrAsm("A=A-1");
			wrAsm("A=A-1");
			wrAsm("M=D");

			compareCount++;
		}

		wrAsm("@SP");
		wrAsm("M=M-1");
	}
	wrAsm("");
}

static const std::unordered_map<std::string, std::string> segmentSymbol{
	// 这四个内存地址中的值才是对应段的基址
	{"local", "LCL"},
	{"argument", "ARG"},
	{"this", "THIS"},
	{"that", "THAT"},

	// 这三个内存地址处就是对应段的基址
	{"pointer", "THIS"},
	{"temp", "R5"},
	{"static", "16"}
};

void CodeWrite::writePushPop(Parser::CMD_TYPE type, const std::string& segment, int index) {
	if (type == Parser::CMD_TYPE::C_PUSH) {
		if (segment == "constant") {
			wrAsm("@" + std::to_string(index));
			wrAsm("D=A");
		}
		else {
			wrAsm("@" + segmentSymbol.at(segment));
			if (segment == "pointer" || segment == "temp" || segment == "static")
				wrAsm("D=A");
			else
				wrAsm("D=M");
			wrAsm("@" + std::to_string(index));
			wrAsm("A=D+A");
			wrAsm("D=M");
		}

		wrAsm("@SP");
		wrAsm("A=M");
		wrAsm("M=D");
		wrAsm("@SP");
		wrAsm("M=M+1");
	}
	else {
		wrAsm("@" + segmentSymbol.at(segment));
		if (segment == "pointer" || segment == "temp" || segment == "static")
			wrAsm("D=A");
		else
			wrAsm("D=M");
		wrAsm("@" + std::to_string(index));
		wrAsm("D=D+A");
		wrAsm("@SP");
		wrAsm("A=M");
		wrAsm("M=D");

		wrAsm("A=A-1");
		wrAsm("D=M");
		wrAsm("A=A+1");
		wrAsm("A=M");
		wrAsm("M=D");

		wrAsm("@SP");
		wrAsm("M=M-1");
	}
	wrAsm("");
}

void CodeWrite::close() {
	fs.close();
}

void CodeWrite::wrAsm(const std::string& asmCMD) {
	fs << asmCMD << std::endl;
}

void CodeWrite::initialize() {
	compareCount = 0;
}