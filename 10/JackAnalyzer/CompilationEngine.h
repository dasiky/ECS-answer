#pragma once
#include "JackTokenizer.h"
#include <vector>
#include <fstream>
#include <initializer_list>

class CompilationEngine {
public:
	CompilationEngine(JackTokenizer& tokenizer, const std::string& outPath);

	void compileClass();
private:
	bool compileClassVarDec();
	bool compileSubroutineDec();
	bool compileSubroutineBody();
	bool compileParameterList();
	bool compileVarDec();
	bool compileStatements();
	bool compileDo();
	bool compileLet();
	bool compileWhile();
	bool compileReturn();
	bool compileIf();
	bool compileExpression();
	bool compileTerm();
	bool compileExpressionList();

	bool isType(int index = -1);
	bool isTokenEqual(TOKEN_TYPE type, const std::initializer_list<std::string>& vals);
	TOKEN_TYPE curType();
	std::string curToken();
	std::vector<std::pair<TOKEN_TYPE, std::string>> tokens;
	int index = 0;

	void prtTermSym();
	void prtNTermSym(const std::string& tagName, bool isEnd);
	int level = 0;
	std::ofstream ofs;
};