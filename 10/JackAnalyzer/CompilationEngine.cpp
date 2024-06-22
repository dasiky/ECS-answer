#include "CompilationEngine.h"
#include <iostream>

CompilationEngine::CompilationEngine(JackTokenizer& tokenizer, const std::string& outPath) {
	while (tokenizer.hasMoreTokens()) {
		tokenizer.advance();
		tokens.emplace_back(tokenizer.tokenType(), tokenizer.tokenVal());
	}

	ofs.open(outPath);
	if (!ofs.is_open())
		std::cerr << "can't open file: " << outPath << std::endl;
}

void CompilationEngine::compileClass() {
	prtNTermSym("class", false);

	if (!isTokenEqual(TOKEN_TYPE::keyword, { "class" })) return;
	prtTermSym(); index++;

	if (!isTokenEqual(TOKEN_TYPE::identifier, {})) return;
	prtTermSym(); index++;

	if (!isTokenEqual(TOKEN_TYPE::symbol, {"{"})) return;
	prtTermSym();

	while (compileClassVarDec());
	while (compileSubroutineDec());

	index++;
	if (!isTokenEqual(TOKEN_TYPE::symbol, { "}" })) return;
	prtTermSym();

	prtNTermSym("class", true);
	ofs.close();
}

bool CompilationEngine::compileClassVarDec() {
	prtNTermSym("classVarDec", false);
	index++;

	if (!isTokenEqual(TOKEN_TYPE::keyword, { "static", "field" })) {
		index--;  return false;
	}
	prtTermSym(); index++;

	if (!isType()) return false;
	prtTermSym(); index++;

	if (!isTokenEqual(TOKEN_TYPE::identifier, {})) return false;
	prtTermSym(); index++;
	
	while (isTokenEqual(TOKEN_TYPE::symbol, { "," })) {
		prtTermSym(); index++;
		if (!isTokenEqual(TOKEN_TYPE::identifier, {})) return false;
		prtTermSym(); index++;
	}

	if (!isTokenEqual(TOKEN_TYPE::symbol, { ";" })) return false;

	prtNTermSym("classVarDec", true);
	return true;
}

bool CompilationEngine::compileSubroutineDec() {
	prtNTermSym("subroutineDec", false);
	index++;

	if (!isTokenEqual(TOKEN_TYPE::keyword, { "constructor", "function", "method" })) {
		index--;  return false;
	}
	prtTermSym(); index++;

	if (!isTokenEqual(TOKEN_TYPE::keyword, { "void" }) && !isType()) return false;
	prtTermSym(); index++;

	if (!isTokenEqual(TOKEN_TYPE::identifier, {})) return false;
	prtTermSym(); index++;

	if (!isTokenEqual(TOKEN_TYPE::symbol, { "(" })) return false;
	prtTermSym();

	if (!compileParameterList()) return false;
	index++;

	if (!isTokenEqual(TOKEN_TYPE::symbol, { ")" })) return false;
	prtTermSym();

	if (!compileSubroutineBody()) return false;

	prtNTermSym("subroutineDec", true);
	return true;
}

bool CompilationEngine::compileSubroutineBody() {
	prtNTermSym("subroutineBody", false);
	index++;

	if (!isTokenEqual(TOKEN_TYPE::symbol, { "{" })) {
		index--;  return false;
	}
	prtTermSym();

	while (compileVarDec());
	if (!compileStatements()) return false;

	index++;
	if (!isTokenEqual(TOKEN_TYPE::symbol, { "}" })) return false;
	prtTermSym();

	prtNTermSym("subroutineBody", true);
	return true;
}

bool CompilationEngine::compileParameterList() {
	prtNTermSym("parameterList", false);
	index++;

	if (!isType()) { index--; return true; }
	prtTermSym(); index++;

	if (!isTokenEqual(TOKEN_TYPE::identifier, {})) return false;
	prtTermSym(); index++;

	while (isTokenEqual(TOKEN_TYPE::symbol, { "," })) {
		if (!isType()) return false;
		prtTermSym(); index++;
		if (!isTokenEqual(TOKEN_TYPE::identifier, {})) return false;
		prtTermSym(); index++;
	}
	index--;

	prtNTermSym("parameterList", true);
	return true;
}

bool CompilationEngine::compileVarDec() {
	prtNTermSym("varDec", false);
	index++;

	if (!isTokenEqual(TOKEN_TYPE::keyword, { "var" })) {
		index--;  return false;
	}
	prtTermSym(); index++;

	if (!isType()) return false;
	prtTermSym(); index++;

	if (!isTokenEqual(TOKEN_TYPE::identifier, {})) return false;
	prtTermSym(); index++;

	while (isTokenEqual(TOKEN_TYPE::symbol, { "," })) {
		prtTermSym(); index++;
		if (!isTokenEqual(TOKEN_TYPE::identifier, {})) return false;
		prtTermSym(); index++;
	}

	if (!isTokenEqual(TOKEN_TYPE::symbol, { ";" })) return false;
	prtTermSym();

	prtNTermSym("varDec", true);
	return true;
}

bool CompilationEngine::compileStatements() {
	prtNTermSym("statements", false);
	index++;

	for (; isTokenEqual(TOKEN_TYPE::keyword, { "let", "if", "while", "do", "return" }); index++) {
		index--;
		if (compileLet()) continue;
		if (compileIf()) continue;
		if (compileWhile()) continue;
		if (compileDo()) continue;
		if (compileReturn()) continue;
		return false;
	}

	prtNTermSym("statements", true);
	return true;
}

bool CompilationEngine::compileDo() {
	prtNTermSym("doStatement", false);
	index++;

	if (!isTokenEqual(TOKEN_TYPE::keyword, { "do" })) {
		index--; return false;
	}
	prtTermSym(); index++;

	if (!isTokenEqual(TOKEN_TYPE::identifier, {})) return false;
	prtTermSym(); index++;

	if (!isTokenEqual(TOKEN_TYPE::symbol, { "(", "."})) return false;
	prtTermSym();

	if (curToken() == "(") {
		if (!compileExpressionList()) return false;

		if (!isTokenEqual(TOKEN_TYPE::symbol, { ")" })) return false;
		prtTermSym();
	}
	else {
		index++;
		if (!isTokenEqual(TOKEN_TYPE::identifier, {})) return false;
		prtTermSym(); index++;

		if (!isTokenEqual(TOKEN_TYPE::symbol, { "(" })) return false;
		prtTermSym();

		if (!compileExpressionList()) return false;

		if (!isTokenEqual(TOKEN_TYPE::symbol, { ")" })) return false;
		prtTermSym();
	}

	prtNTermSym("doStatement", true);
	return true;
}

bool CompilationEngine::compileLet() {
	prtNTermSym("letStatement", false);
	index++;

	if (!isTokenEqual(TOKEN_TYPE::keyword, { "let" })) {
		index--; return false;
	}

	if (!isTokenEqual(TOKEN_TYPE::identifier, {})) return false;
	prtTermSym(); index++;

	if (isTokenEqual(TOKEN_TYPE::symbol, { "[" })) {
		prtTermSym();

		if (!compileExpression()) return false;
		index++;

		if (!isTokenEqual(TOKEN_TYPE::symbol, { "[" })) return false;
		prtTermSym(); index++;
	}
	
	if (!isTokenEqual(TOKEN_TYPE::symbol, { "=" })) return false;
	prtTermSym();

	if (!compileExpression()) return false;
	index++;

	if (!isTokenEqual(TOKEN_TYPE::symbol, { ";" })) return false;
	prtTermSym();

	prtNTermSym("letStatement", true);
	return true;
}

bool CompilationEngine::compileWhile() {
	prtNTermSym("whileStatement", false);
	index++;

	if (!isTokenEqual(TOKEN_TYPE::keyword, { "while" })) {
		index--; return false;
	}
	prtTermSym(); index++;

	if (!isTokenEqual(TOKEN_TYPE::symbol, { "(" })) return false;
	prtTermSym();

	if (!compileExpression()) return false;
	index++;

	if (!isTokenEqual(TOKEN_TYPE::symbol, { ")" })) return false;
	prtTermSym(); index++;

	if (!isTokenEqual(TOKEN_TYPE::symbol, { "{" })) return false;
	prtTermSym();

	if (!compileStatements()) return false;
	index++;

	if (!isTokenEqual(TOKEN_TYPE::symbol, { "}" })) return false;
	prtTermSym();

	prtNTermSym("whileStatement", true);
	return true;
}

bool CompilationEngine::compileReturn() {
	prtNTermSym("returnStatement", false);
	index++;

	if (!isTokenEqual(TOKEN_TYPE::keyword, { "return" })) {
		index--; return false;
	}
	prtTermSym();

	compileExpression();
	index++;
	
	if (!isTokenEqual(TOKEN_TYPE::symbol, { ";" })) return false;
	prtTermSym();

	prtNTermSym("returnStatement", true);
	return true;
}

bool CompilationEngine::compileIf() {
	prtNTermSym("ifStatement", false);
	index++;

	if (!isTokenEqual(TOKEN_TYPE::keyword, { "if" })) {
		index--; return false;
	}
	prtTermSym(); index++;

	if (!isTokenEqual(TOKEN_TYPE::symbol, { "(" })) return false;
	prtTermSym();

	if (!compileExpression()) return false;
	index++;

	if (!isTokenEqual(TOKEN_TYPE::symbol, { ")" })) return false;
	prtTermSym(); index++;

	if (!isTokenEqual(TOKEN_TYPE::symbol, { "{" })) return false;
	prtTermSym();

	if (!compileStatements()) return false;
	index++;

	if (!isTokenEqual(TOKEN_TYPE::symbol, { "}" })) return false;
	prtTermSym();

	prtNTermSym("ifStatement", true);
	return true;
}

bool CompilationEngine::compileExpression() {
	prtNTermSym("expression", false);
	
	if (!compileTerm()) return false;
	index++;
	
	while (isTokenEqual(TOKEN_TYPE::symbol, { "+", "-", "*", "/", "&", "|", "<", ">", "=" })) {
		prtTermSym();
		if (!compileTerm()) return false;
		index++;
	}

	prtNTermSym("expression", true);
	return true;
}

bool CompilationEngine::compileTerm() {
	prtNTermSym("term", false);
	index++;

	if (curType() == TOKEN_TYPE::int_const ||
		curType() == TOKEN_TYPE::string_const ||
		isTokenEqual(TOKEN_TYPE::keyword, { "true", "false", "null", "this" }))
		prtTermSym();
	else if (curType() == TOKEN_TYPE::identifier) {
		prtTermSym();
		index++;
		if (isTokenEqual(TOKEN_TYPE::symbol, { "(" , "."})) {
			// same as compileDo()
			prtTermSym();
			if (curToken() == "(") {
				if (!compileExpressionList()) return false;

				if (!isTokenEqual(TOKEN_TYPE::symbol, { ")" })) return false;
				prtTermSym();
			}
			else {
				index++;
				if (!isTokenEqual(TOKEN_TYPE::identifier, {})) return false;
				prtTermSym(); index++;

				if (!isTokenEqual(TOKEN_TYPE::symbol, { "(" })) return false;
				prtTermSym();

				if (!compileExpressionList()) return false;

				if (!isTokenEqual(TOKEN_TYPE::symbol, { ")" })) return false;
				prtTermSym();
			}
		}
		else if (isTokenEqual(TOKEN_TYPE::symbol, { "[" })) {
			prtTermSym();
			if (!compileExpression()) return false;
			index++;
			if (!isTokenEqual(TOKEN_TYPE::symbol, { "]" })) return false;
		}
	}
	else if (isTokenEqual(TOKEN_TYPE::symbol, { "(" })) {
		prtTermSym();
		if (!compileExpression()) return false;
		index++;

		if (!isTokenEqual(TOKEN_TYPE::symbol, { ")" })) return false;
	}
	else if (isTokenEqual(TOKEN_TYPE::symbol, { "-", "~" })) {
		prtTermSym();
		if (!compileTerm()) return false;
	}
	else
		return false;

	prtNTermSym("term", true);
	return true;
}

bool CompilationEngine::compileExpressionList() {
	prtNTermSym("expressionList", false);
	
	if (!compileExpression()) return true;
	index++;

	while (isTokenEqual(TOKEN_TYPE::symbol, { "," })) {
		prtTermSym();
		if (!compileExpression()) return false;
		index++;
	}
	index--;
	prtNTermSym("expressionList", true);
	return true;
}

bool CompilationEngine::isType(int index) {
	if (index == -1)
		index = this->index;

	return isTokenEqual(TOKEN_TYPE::keyword, { "int", "char", "boolean" }) || isTokenEqual(TOKEN_TYPE::identifier, {});
}

bool CompilationEngine::isTokenEqual(TOKEN_TYPE type, const std::initializer_list<std::string>& vals, int index) {
	if (index == -1) 
		index = this->index;

	if (curType() != type)
		return false;
	if (vals.size() == 0)
		return true;
	for (auto val : vals)
		if (curToken() == val)
			return true;
	return false;
}

TOKEN_TYPE CompilationEngine::curType() {
	return tokens[index].first;
}

std::string CompilationEngine::curToken() {
	return tokens[index].second;
}

void CompilationEngine::prtTermSym() {
	for (int i = 0; i < level; i++)
		ofs << "  ";

	ofs << "<" + to_string(curType()) + "> " + curToken() + " </" + to_string(curType()) + ">" << std::endl;
}

void CompilationEngine::prtNTermSym(const std::string& tagName, bool isEnd) {
	for (int i = 0; i < level; i++)
		ofs << "  ";

	if (isEnd) {
		ofs << "</" + tagName + ">" << std::endl;
		level--;
	}
	else {
		ofs << "<" + tagName + ">" << std::endl;
		level++;
	}
}
