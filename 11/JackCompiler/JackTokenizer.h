#pragma once
#include <string>
#include <vector>

enum class TOKEN_TYPE
{
    keyword,
    symbol,
    identifier,
    int_const,
    string_const,
    error
};
std::string to_string(TOKEN_TYPE type);

class JackTokenizer {
public:
    JackTokenizer(const std::string& filepath);
    bool hasMoreTokens();
    void advance();

    TOKEN_TYPE tokenType();
    std::string tokenVal();
    std::string keyWord();
    char symbol();
    std::string identifier();
    int intVal();
    std::string stringVal();
private:
    std::string code;
    int codeIndex = 0;
    bool advanceFlag = true;
    std::string curTokenVal;
    TOKEN_TYPE curTokenType{TOKEN_TYPE::error};

    bool getKeyword();
    bool getSymbol();
    bool getIntegerConstant();
    bool getStringConstant();
    bool getIdentifier();
};