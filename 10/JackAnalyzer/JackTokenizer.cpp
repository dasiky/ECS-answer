#include "JackTokenizer.h"
#include <iostream>
#include <fstream>
#include <sstream>

bool JackTokenizer::getKeyword() {
    static const std::vector<std::string> keywords{
        "class",
        "constructor",
        "function",
        "method",
        "field",
        "static",
        "var",
        "int",
        "char",
        "boolean",
        "void",
        "true",
        "false",
        "null",
        "this",
        "let",
        "do",
        "if",
        "else",
        "while",
        "return"
    };
    for (auto keyword : keywords) {
        if (code.substr(codeIndex, keyword.size()) == keyword) {
            tokens.emplace_back(TOKEN_TYPE::keyword, keyword);
            codeIndex += keyword.size();
            return true;
        }
    }
    return false;
}

bool JackTokenizer::getSymbol() {
    static const std::string symbols = "{}()[].,;+-*/&|<>=~";
    for (auto c : symbols)
        if (code[codeIndex] == c) {
            tokens.emplace_back(TOKEN_TYPE::symbol, std::string(1, c));
            codeIndex++;
            return true;
        }
    return false;
}

bool JackTokenizer::getIntegerConstant() {
    int i = codeIndex;
    while (i < code.size() && isdigit(code[i]))
        i++;
    if (i == codeIndex)
        return false;

    tokens.emplace_back(TOKEN_TYPE::int_const, code.substr(codeIndex, i - codeIndex));
    codeIndex += i - codeIndex;
    return true;
}

bool JackTokenizer::getStringConstant() {
    if (code[codeIndex] != '\"')
        return false;
    auto nextIndex = code.find_first_of('\"', codeIndex + 1);
    if (nextIndex == std::string::npos)
        return false;
    tokens.emplace_back(TOKEN_TYPE::string_const, code.substr(codeIndex + 1, nextIndex - codeIndex - 1));
    codeIndex += nextIndex - codeIndex + 1;
    return true;
}

bool JackTokenizer::getIdentifier() {
    if (!isalpha(code[codeIndex]) && code[codeIndex] != '_')
        return false;

    int i = codeIndex;
    while (i < code.size() && (isalnum(code[i]) || code[i] == '_'))
        i++;

    if (i == codeIndex)
        return false;

    tokens.emplace_back(TOKEN_TYPE::identifier, code.substr(codeIndex, i - codeIndex));
    codeIndex += i - codeIndex;
    return true;
}

JackTokenizer::JackTokenizer(const std::string& filepath) {
    auto ifs = std::ifstream(filepath);
    if (!ifs.is_open()) {
        std::cerr << "can't open file: " << filepath << std::endl;
        return;
    }

    std::string codeTemp = (std::stringstream() << ifs.rdbuf()).str();
    // delete comment
    for (int i = 0; i < codeTemp.size() - 1;) {
        while (i < codeTemp.size() - 1 && codeTemp.substr(i, 2) != "//") {
            code.push_back(codeTemp[i++]);
        }
        int j = i + 2;
        while (j < codeTemp.size() && codeTemp[j] != '\n')
            j++;
        i = j;
    }
    code.push_back(codeTemp.back());
    codeTemp = code;
    code.clear();
    for (int i = 0; i < codeTemp.size() - 1;) {
        while (i < codeTemp.size() - 1 && codeTemp.substr(i, 2) != "/*") {
            code.push_back(codeTemp[i++]);
        }
        int j = i + 2;
        while (j < codeTemp.size() - 1 && codeTemp.substr(j, 2) != "*/")
            j++;
        i = j + 2;
    }
    code.push_back(codeTemp.back());
    std::cout << code << std::endl;
    
    std::string token;
    while (codeIndex < code.size()) {
        while (codeIndex < code.size() && isspace(code[codeIndex]))
            codeIndex++;
        if (getKeyword()) continue;
        if (getSymbol()) continue;
        if (getIntegerConstant()) continue;
        if (getStringConstant()) continue;
        if (getIdentifier()) continue;
        return;
    }
}

bool JackTokenizer::hasMoreTokens() {
    return tokenIndex < tokens.size() - 1;
}

void JackTokenizer::advance() {
    tokenIndex++;
}

TOKEN_TYPE JackTokenizer::tokenType() {
    return tokens[tokenIndex].type;
}

std::string JackTokenizer::keyWord() {
    return tokens[tokenIndex].val;
}

char JackTokenizer::symbol() {
    return tokens[tokenIndex].val[0];
}

std::string JackTokenizer::identifier() {
    return tokens[tokenIndex].val;
}

int JackTokenizer::intVal() {
    return atoi(tokens[tokenIndex].val.c_str());
}

std::string JackTokenizer::stringVal() {
    return tokens[tokenIndex].val;
}

std::string JackTokenizer::tokenVal() {
    return tokens[tokenIndex].val;
}

std::string to_string(TOKEN_TYPE type) {
    switch (type)
    {
    case TOKEN_TYPE::keyword:
        return "keyword";
    case TOKEN_TYPE::symbol:
        return "symbol";
    case TOKEN_TYPE::identifier:
        return "identifier";
    case TOKEN_TYPE::int_const:
        return "integerConstant";
    case TOKEN_TYPE::string_const:
        return "stringConstant";
    }
}