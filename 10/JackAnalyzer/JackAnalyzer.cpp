#include "JackTokenizer.h"
#include <fstream>
#include <filesystem>
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2)
        return 0;

    std::string jackPath = argv[1];
    std::string jackFilename = std::filesystem::path(jackPath).stem().string();
    std::string directoryPath = std::filesystem::path(jackPath).parent_path().string();
    std::string tokenPath = directoryPath + "\\" + jackFilename + "T2.xml";
    std::string analPath = directoryPath + "\\" + jackFilename + "2.xml";
    std::cout << "output: " << std::endl;
    std::cout << tokenPath << std::endl;

    std::ofstream tokenOfs(tokenPath);
    tokenOfs << "<tokens>" << std::endl;

    auto tokenizer = JackTokenizer(jackPath);
    while (tokenizer.hasMoreTokens()) {
        tokenizer.advance();
        auto type = tokenizer.tokenType();
        auto tokenVal = tokenizer.tokenVal();
        if (tokenVal == ">")
            tokenVal = "&gt;";
        if (tokenVal == "<")
            tokenVal = "&lt;";
        if (tokenVal == "&")
            tokenVal = "&amp;";
        tokenOfs << "<" + to_string(type) + "> " + tokenVal + " </" + to_string(type) + ">" << std::endl;
    }

    tokenOfs << "</tokens>" << std::endl;
    tokenOfs.close();
}