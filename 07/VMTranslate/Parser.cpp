#include "Parser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

Parser::Parser(const std::vector<std::string>& filenames) {
    for (auto filename: filenames)
    {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "can't open file." << filename << std::endl;
            return;
        }
        std::string f = filename.substr(filename.find_last_of("\\") + 1);
        f = f.substr(0, f.size() - 3);

        std::string line;
        while (std::getline(file, line)) {
            auto comment = line.find("//");
            line = line.substr(0, comment);
            int start = 0, end = line.size() - 1;
            while (start < line.size() && isspace(line[start]))
                start++;
            while (end >= 0 && isspace(line[end]))
                end--;
            line = line.substr(start, end - start + 1);
            if (line == "") continue;

            std::stringstream ss(line);
            source.emplace_back();
            std::string temp;
            while (ss >> temp)
                source.back().push_back(temp);
            source.back().push_back(f);
        };
        file.close();
    }
}

bool Parser::hasMoreCommands() {
    return index < source.size() - 1;
}

void Parser::advance() {
    index++;
}

Parser::CMD_TYPE Parser::commandType() {
    static const std::unordered_map<std::string, CMD_TYPE> mp{
        {"add", CMD_TYPE::C_ARITHMETIC},
        {"sub", CMD_TYPE::C_ARITHMETIC},
        {"neg", CMD_TYPE::C_ARITHMETIC},
        {"eq", CMD_TYPE::C_ARITHMETIC},
        {"gt", CMD_TYPE::C_ARITHMETIC},
        {"lt", CMD_TYPE::C_ARITHMETIC},
        {"and", CMD_TYPE::C_ARITHMETIC},
        {"or", CMD_TYPE::C_ARITHMETIC},
        {"not", CMD_TYPE::C_ARITHMETIC},

        {"push", CMD_TYPE::C_PUSH},
        {"pop", CMD_TYPE::C_POP},
        
        // project 8
        {"label", CMD_TYPE::C_LABEL},
        {"goto", CMD_TYPE::C_GOTO},
        {"if-goto", CMD_TYPE::C_IF},
        {"function", CMD_TYPE::C_FUNCTION},
        {"call", CMD_TYPE::C_CALL},
        {"return", CMD_TYPE::C_RETURN},
    };


    return mp.at(source[index][0]);
}

std::string Parser::cmd() {
    return source[index][0];
}

std::string Parser::arg1() {
    if (source.size() >= 1)
        return source[index][1];
    return "";
}

std::string Parser::arg2() {
    if (source.size() >= 2)
        return source[index][2];
    return "";
}

std::string Parser::filename() {
    return source[index].back();
}