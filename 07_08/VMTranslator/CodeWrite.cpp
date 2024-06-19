#include "CodeWrite.h"
#include <iostream>
#include <unordered_map>

void CodeWrite::setFilename(const std::string& filename) {
    fs = std::ofstream(filename);
    if (!fs.is_open()) {
        std::cerr << "can't open file." << filename << std::endl;
        return;
    }
}

void CodeWrite::close() {
    fs.close();
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
            wrAsm("0;JMP");
            wrAsm("(" + trueSymbol + ")");
            wrAsm("D=-1");
            wrAsm("(" + endSymbol + ")");

            wrAsm("@SP");
            wrAsm("A=M");
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
    // 这四个内存地址中的值才是对应段的基址，即
    // @x
    // A=M
    // 此时A才指向该段
    {"local", "LCL"},
    {"argument", "ARG"},
    {"this", "THIS"},
    {"that", "THAT"},

    // 这三个内存地址处就是对应段的基址，即
    // @x
    // 此时A就指向该段
    {"pointer", "THIS"},
    {"temp", "R5"},
    {"static", "16"}
};

void CodeWrite::writePushPop(Parser::CMD_TYPE type, const std::string& segment, int index, const std::string& filename) {
    if (type == Parser::CMD_TYPE::C_PUSH) {
        if (segment == "constant") {
            wrAsm("@" + std::to_string(index));
            wrAsm("D=A");
        }
        else if (segment == "static") {
            wrAsm("@" + filename + "." + std::to_string(index));
            wrAsm("D=M");
        }
        else {
            wrAsm("@" + segmentSymbol.at(segment));
            if (segment == "pointer" || segment == "temp")
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
        if (segment == "static") {
            wrAsm("@" + filename + "." + std::to_string(index));
            wrAsm("D=A");
        }
        else {
            wrAsm("@" + segmentSymbol.at(segment));
            if (segment == "pointer" || segment == "temp")
                wrAsm("D=A");
            else
                wrAsm("D=M");
            wrAsm("@" + std::to_string(index));
            wrAsm("D=D+A");
        }
        wrAsm("@R13");
        wrAsm("M=D");

        wrAsm("@SP");
        wrAsm("A=M");
        wrAsm("A=A-1");
        wrAsm("D=M");
        
        wrAsm("@R13");
        wrAsm("A=M");
        wrAsm("M=D");

        wrAsm("@SP");
        wrAsm("M=M-1");
    }
    wrAsm("");
}

void CodeWrite::writeInit() {
    wrAsm("@256");
    wrAsm("D=A");
    wrAsm("@SP");
    wrAsm("M=D");
    writeCall("Sys.init", 0);
}

void CodeWrite::writeLabel(const std::string& label) {
    std::string asmLabel = nowFunc + "$" + label;
    wrAsm("(" + asmLabel + ")");
}

void CodeWrite::writeGoto(const std::string& label) {
    std::string asmLabel = nowFunc + "$" + label;
    wrAsm("@" + asmLabel);
    wrAsm("0;JMP");
    wrAsm("");
}

void CodeWrite::writeIf(const std::string& label) {
    std::string asmLabel = nowFunc + "$" + label;
    wrAsm("@SP");
    wrAsm("A=M-1");
    wrAsm("D=M");
    wrAsm("@SP");
    wrAsm("M=M-1");
    wrAsm("@" + asmLabel);
    wrAsm("D;JNE");
    wrAsm("");
}

void CodeWrite::writeCall(const std::string& funcName, int numArgs) {
    std::string returnSymbol = "_return_" + std::to_string(returnCount);
    wrAsm("@" + returnSymbol);
    wrAsm("D=A");
    wrAsm("@SP");
    wrAsm("A=M");
    wrAsm("M=D");
    wrAsm("@SP");
    wrAsm("M=M+1");

    for (std::string p : std::vector{ "LCL", "ARG", "THIS", "THAT" }) {
        wrAsm("@" + p);
        wrAsm("D=M");
        wrAsm("@SP");
        wrAsm("A=M");
        wrAsm("M=D");
        wrAsm("@SP");
        wrAsm("M=M+1");
    }

    wrAsm("@SP");
    wrAsm("D=M");
    wrAsm("@" + std::to_string(5 + numArgs));
    wrAsm("D=D-A");
    wrAsm("@ARG");
    wrAsm("M=D");

    wrAsm("@SP");
    wrAsm("D=M");
    wrAsm("@LCL");
    wrAsm("M=D");

    wrAsm("@" + funcName);
    wrAsm("0;JMP");

    wrAsm("(" + returnSymbol + ")");

    wrAsm("");
    returnCount++;
}

void CodeWrite::writeReturn() {
    for (std::string p : std::vector{ "THAT", "THIS", "R13", "R14", "R15" }) {
        wrAsm("@LCL");
        wrAsm("A=M-1");
        wrAsm("D=M");
        wrAsm("@" + p);
        wrAsm("M=D");
        wrAsm("@LCL");
        wrAsm("M=M-1");
    }
    // return value
    wrAsm("@SP");
    wrAsm("A=M-1");
    wrAsm("D=M");
    wrAsm("@ARG");
    wrAsm("A=M");
    wrAsm("M=D");

    wrAsm("@ARG");
    wrAsm("D=M");
    wrAsm("@SP");
    wrAsm("M=D+1");

    wrAsm("@R13");
    wrAsm("D=M");
    wrAsm("@ARG");
    wrAsm("M=D");

    wrAsm("@R14");
    wrAsm("D=M");
    wrAsm("@LCL");
    wrAsm("M=D");

    wrAsm("@R15");
    wrAsm("A=M");
    wrAsm("0;JMP");
    wrAsm("");
}

void CodeWrite::writeFunction(std::string funcName, int numLocals) {
    wrAsm("(" + funcName + ")");
    for (int i = 0; i < numLocals; i++) {
        wrAsm("@SP");
        wrAsm("A=M");
        wrAsm("M=0");
        wrAsm("@SP");
        wrAsm("M=M+1");
    }
    wrAsm("");

    nowFunc = funcName;
}

void CodeWrite::wrAsm(const std::string& asmCMD) {
    fs << asmCMD << std::endl;
}