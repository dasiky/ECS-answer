#pragma once
#include "JackTokenizer.h"

class CompilationEngine {
public:
	CompilationEngine(JackTokenizer& tokenizer);

	void compileClass();
	void compileClassVarDec();
	void compileSubroutine();
	void compileParameterList();
	void compileVarDec();
	void compileStatements();
	void compileDo();
	void compileLet();
	void compileWhile();
	void compileReturn();
	void compileIf();
	void compileExpression();
	void compileTerm();
	void compileExpressionList();
private:
	JackTokenizer& tokenizer;
};