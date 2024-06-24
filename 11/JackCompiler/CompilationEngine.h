#pragma once
#include "JackTokenizer.h"
#include "SymbolTabel.h"
#include "VMWriter.h"
#include <vector>
#include <fstream>
#include <initializer_list>

class CompilationEngine {
public:
	CompilationEngine(JackTokenizer& tokenizer, const std::string& outPath, VMWriter& writer);

	void compileClass();
private:
	bool compileClassVarDec(int& fieldCount);
	bool compileSubroutineDec(const int fieldCount);
	bool compileSubroutineBody(const int fieldCount);
	bool compileParameterList();
	bool compileVarDec(int& varCount);
	bool compileStatements();
	bool compileDo();
	bool compileSubRoutineCall();
	bool compileLet();
	bool compileWhile();
	bool compileReturn();
	bool compileIf();
	bool compileExpression();
	bool compileTerm();
	bool compileExpressionList(int& expCount);

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

	std::string className;
	std::string funcName;
	std::string subroutineType;
	int labelCount = 0;
	SymbolTabel symTabel;
	VMWriter& writer;
};