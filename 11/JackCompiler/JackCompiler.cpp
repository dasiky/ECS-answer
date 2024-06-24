#include "JackTokenizer.h"
#include "CompilationEngine.h"
#include "VMWriter.h"
#include <fstream>
#include <filesystem>
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2)
        return 0;

    std::string jackPath = argv[1];
    std::string jackFilename = std::filesystem::path(jackPath).stem().string();
    std::string directoryPath = std::filesystem::path(jackPath).parent_path().string();
    std::string compilePath = directoryPath + "\\" + jackFilename + ".vm";
    std::string analPath = directoryPath + "\\" + jackFilename + ".xml";
    std::cout << "output: " << std::endl;
    std::cout << compilePath << std::endl;
    auto tokenizer = JackTokenizer(jackPath);
    auto writer = VMWriter(compilePath);
    auto engine = CompilationEngine(tokenizer, analPath, writer);
    engine.compileClass();
}