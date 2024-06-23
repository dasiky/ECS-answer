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
	index++;
	if (!isTokenEqual(TOKEN_TYPE::keyword, { "static", "field" })) {
		index--;  return false;
	}
	prtNTermSym("classVarDec", false);
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

	prtNTermSym("classVarDec", true);
	return true;
}

bool CompilationEngine::compileSubroutineDec() {
	index++;
	if (!isTokenEqual(TOKEN_TYPE::keyword, { "constructor", "function", "method" })) {
		index--;  return false;
	}
	prtNTermSym("subroutineDec", false);
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
	index++;
	if (!isTokenEqual(TOKEN_TYPE::symbol, { "{" })) {
		index--;  return false;
	}
	prtNTermSym("subroutineBody", false);
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

	if (!isType()) { index--; prtNTermSym("parameterList", true); return true; }
	prtTermSym(); index++;

	if (!isTokenEqual(TOKEN_TYPE::identifier, {})) return false;
	prtTermSym(); index++;

	while (isTokenEqual(TOKEN_TYPE::symbol, { "," })) {
		prtTermSym(); index++;
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
	index++;
	if (!isTokenEqual(TOKEN_TYPE::keyword, { "var" })) {
		index--;  return false;
	}
	prtNTermSym("varDec", false);
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
	index--;

	prtNTermSym("statements", true);
	return true;
}

bool CompilationEngine::compileDo() {
	index++;
	if (!isTokenEqual(TOKEN_TYPE::keyword, { "do" })) {
		index--; return false;
	}
	prtNTermSym("doStatement", false);
	prtTermSym(); index++;

	if (!isTokenEqual(TOKEN_TYPE::identifier, {})) return false;
	prtTermSym(); index++;

	if (!isTokenEqual(TOKEN_TYPE::symbol, { "(", "."})) return false;
	prtTermSym();

	if (curToken() == "(") {
		if (!compileExpressionList()) return false;
		index++;
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

		index++;
		if (!isTokenEqual(TOKEN_TYPE::symbol, { ")" })) return false;
		prtTermSym();
	}
	index++;
	if (!isTokenEqual(TOKEN_TYPE::symbol, { ";" })) return false;
	prtTermSym();

	prtNTermSym("doStatement", true);
	return true;
}

bool CompilationEngine::compileLet() {
	index++;

	if (!isTokenEqual(TOKEN_TYPE::keyword, { "let" })) {
		index--; return false;
	}
	prtNTermSym("letStatement", false);
	prtTermSym(); index++;

	if (!isTokenEqual(TOKEN_TYPE::identifier, {})) return false;
	prtTermSym(); index++;

	if (isTokenEqual(TOKEN_TYPE::symbol, { "[" })) {
		prtTermSym();

		if (!compileExpression()) return false;
		index++;

		if (!isTokenEqual(TOKEN_TYPE::symbol, { "]" })) return false;
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
	index++;

	if (!isTokenEqual(TOKEN_TYPE::keyword, { "while" })) {
		index--; return false;
	}
	prtNTermSym("whileStatement", false);
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
	index++;

	if (!isTokenEqual(TOKEN_TYPE::keyword, { "return" })) {
		index--; return false;
	}
	prtNTermSym("returnStatement", false);
	prtTermSym(); index++;

	if (curType() == TOKEN_TYPE::identifier ||
		isTokenEqual(TOKEN_TYPE::symbol, { "-", "~" }) ||
		curType() == TOKEN_TYPE::int_const ||
		curType() == TOKEN_TYPE::string_const ||
		isTokenEqual(TOKEN_TYPE::keyword, { "true", "false", "null", "this" })) {
		index--;
		if (!compileExpression()) return false;
	}
	else
		index--;
	index++;
	
	if (!isTokenEqual(TOKEN_TYPE::symbol, { ";" })) return false;
	prtTermSym();

	prtNTermSym("returnStatement", true);
	return true;
}

bool CompilationEngine::compileIf() {
	index++;

	if (!isTokenEqual(TOKEN_TYPE::keyword, { "if" })) {
		index--; return false;
	}
	prtNTermSym("ifStatement", false);
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

	index++;
	if (isTokenEqual(TOKEN_TYPE::keyword, { "else" })) {
		prtTermSym(); index++;

		if (!isTokenEqual(TOKEN_TYPE::symbol, { "{" })) return false;
		prtTermSym();

		if (!compileStatements()) return false;
		index++;

		if (!isTokenEqual(TOKEN_TYPE::symbol, { "}" })) return false;
		prtTermSym();
	}
	else
		index--;

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
	index--;

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
		if (isTokenEqual(TOKEN_TYPE::symbol, { "(" , "." })) {
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

				index++;
				if (!isTokenEqual(TOKEN_TYPE::symbol, { ")" })) return false;
				prtTermSym();
			}
		}
		else if (isTokenEqual(TOKEN_TYPE::symbol, { "[" })) {
			prtTermSym();
			if (!compileExpression()) return false;
			index++;
			if (!isTokenEqual(TOKEN_TYPE::symbol, { "]" })) return false;
			prtTermSym();
		}
		else
			index--;
	}
	else if (isTokenEqual(TOKEN_TYPE::symbol, { "(" })) {
		prtTermSym();
		if (!compileExpression()) return false;
		index++;

		if (!isTokenEqual(TOKEN_TYPE::symbol, { ")" })) return false;
		prtTermSym();
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
	index++;
	if (curType() == TOKEN_TYPE::identifier ||
		isTokenEqual(TOKEN_TYPE::symbol, { "-", "~", "("}) ||
		curType() == TOKEN_TYPE::int_const ||
		curType() == TOKEN_TYPE::string_const ||
		isTokenEqual(TOKEN_TYPE::keyword, { "true", "false", "null", "this" })) {
		index--;
		if (!compileExpression()) return false;
	}
	else {
		index--;
		prtNTermSym("expressionList", true);
		return true;
	}

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

bool CompilationEngine::isTokenEqual(TOKEN_TYPE type, const std::initializer_list<std::string>& vals) {
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

	std::string tokenVal = curToken();
	if (tokenVal == "<") tokenVal = "&lt;";
	if (tokenVal == ">") tokenVal = "&gt;";
	if (tokenVal == "&") tokenVal = "&amp;";
	ofs << "<" + to_string(curType()) + "> " + tokenVal + " </" + to_string(curType()) + ">" << std::endl;
}

void CompilationEngine::prtNTermSym(const std::string& tagName, bool isEnd) {
	if (isEnd) level--;
	for (int i = 0; i < level; i++)
		ofs << "  ";

	if (isEnd) {
		ofs << "</" + tagName + ">" << std::endl;
	}
	else {
		ofs << "<" + tagName + ">" << std::endl;
		level++;
	}
}
