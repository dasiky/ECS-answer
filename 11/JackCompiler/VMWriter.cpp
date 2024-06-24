#include "VMWriter.h"
#include <iostream>

std::string to_string(SEGMENT seg) {
	switch (seg)
	{
	case SEGMENT::CONST:
		return "constant";
	case SEGMENT::ARG:
		return "argument";
	case SEGMENT::LOCAL:
		return "local";
	case SEGMENT::STATIC:
		return "static";
	case SEGMENT::THIS:
		return "this";
	case SEGMENT::THAT:
		return "that";
	case SEGMENT::POINTER:
		return "pointer";
	case SEGMENT::TEMP:
		return "temp";
	}
	return "";
}

std::string to_string(ARITH_CMD cmd) {
	switch (cmd)
	{
	case ARITH_CMD::ADD:
		return "add";
	case ARITH_CMD::SUB:
		return "sub";
	case ARITH_CMD::NEG:
		return "neg";
	case ARITH_CMD::EQ:
		return "eq";
	case ARITH_CMD::GT:
		return "gt";
	case ARITH_CMD::LT:
		return "lt";
	case ARITH_CMD::AND:
		return "and";
	case ARITH_CMD::OR:
		return "or";
	case ARITH_CMD::NOT:
		return "not";
	}
	return "";
}

VMWriter::VMWriter(const std::string& outPath) {
	ofs.open(outPath);
	if (!ofs.is_open()) {
		std::cerr << "can't open file: " << outPath << std::endl;
		return;
	}
}

void VMWriter::writePush(SEGMENT seg, int index) {
	ofs << "push " << to_string(seg) << " " + std::to_string(index) << std::endl;
}

void VMWriter::writePop(SEGMENT seg, int index) {
	ofs << "pop " << to_string(seg) << " " + std::to_string(index) << std::endl;
}

void VMWriter::writeArithmetic(ARITH_CMD cmd) {
	ofs << to_string(cmd) << std::endl;
}

void VMWriter::writeLabel(const std::string& label) {
	ofs << "label " << label << std::endl;
}

void VMWriter::writeGoto(const std::string& label) {
	ofs << "goto " << label << std::endl;
}

void VMWriter::writeIf(const std::string& label) {
	ofs << "if-goto " << label << std::endl;
}

void VMWriter::writeCall(const std::string& name, int nArgs) {
	ofs << "call " << name << " " << std::to_string(nArgs) << std::endl;
}

void VMWriter::writeFunction(const std::string& name, int nArgs) {
	ofs << std::endl << "function " << name << " " << std::to_string(nArgs) << std::endl;
}

void VMWriter::writeReturn() {
	ofs << "return" << std::endl;
}

void VMWriter::close() {
	ofs.close();
}
