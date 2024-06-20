#pragma once
#include <string>
#include <vector>

enum class TOKEN_TYPE
{
    keyword,
    symbol,
    identifier,
    int_const,
    string_const
};

std::string to_string(TOKEN_TYPE type);

struct token {
    token() = default;
    token(TOKEN_TYPE type, std::string val) : type(type), val(val){}
    TOKEN_TYPE type;
    std::string val;
};

class JackTokenizer {
public:
    JackTokenizer(const std::string& filepath);
    bool hasMoreTokens();
    void advance();

    TOKEN_TYPE tokenType();
    std::string keyWord();
    char symbol();
    std::string identifier();
    int intVal();
    std::string stringVal();
    std::string tokenVal();
private:
    std::string code;
    int codeIndex = 0;
    std::vector<token> tokens{token()};
    int tokenIndex = 0;

    bool getKeyword();
    bool getSymbol();
    bool getIntegerConstant();
    bool getStringConstant();
    bool getIdentifier();
};