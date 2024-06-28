#include "CompilationEngine.h"
#include <iostream>

CompilationEngine::CompilationEngine(JackTokenizer& tokenizer, VMWriter& writer):writer(writer) {
	while (tokenizer.hasMoreTokens()) {
		tokenizer.advance();
		tokens.emplace_back(tokenizer.tokenType(), tokenizer.tokenVal());
	}
}

void CompilationEngine::compileClass() {
	if (!isTokenEqual(TOKEN_TYPE::keyword, { "class" })) return;
	index++;

	if (!isTokenEqual(TOKEN_TYPE::identifier, {})) return;
	className = curToken();
	index++;

	if (!isTokenEqual(TOKEN_TYPE::symbol, {"{"})) return;

	symTabel.StartSubroutine();
	int fieldCount = 0;
	while (compileClassVarDec(fieldCount));
	while (compileSubroutineDec(fieldCount));
	symTabel.EndSubroutine();

	index++;
	if (!isTokenEqual(TOKEN_TYPE::symbol, { "}" })) return;
	writer.close();
}

bool CompilationEngine::compileClassVarDec(int& fieldCount) {
	index++;
	if (!isTokenEqual(TOKEN_TYPE::keyword, { "static", "field" })) {
		index--;  return false;
	}
	VAR_KIND symKind = (curToken() == "static" ? VAR_KIND::STATIC : VAR_KIND::FIELD);
	index++;

	if (!isType()) return false;
	std::string symType = curToken();
	index++;

	if (!isTokenEqual(TOKEN_TYPE::identifier, {})) return false;
	symTabel.Define(curToken(), symType, symKind);
	if (symKind == VAR_KIND::FIELD)
		fieldCount++;
	index++;
	
	while (isTokenEqual(TOKEN_TYPE::symbol, { "," })) {
		index++;
		if (!isTokenEqual(TOKEN_TYPE::identifier, {})) return false;
		symTabel.Define(curToken(), symType, symKind);
		if (symKind == VAR_KIND::FIELD)
			fieldCount++;
		index++;
	}

	if (!isTokenEqual(TOKEN_TYPE::symbol, { ";" })) return false;
	return true;
}

bool CompilationEngine::compileSubroutineDec(int fieldCount) {
	index++;
	if (!isTokenEqual(TOKEN_TYPE::keyword, { "constructor", "function", "method" })) {
		index--;  return false;
	}
	subroutineType = curToken();
	index++;

	if (!isTokenEqual(TOKEN_TYPE::keyword, { "void" }) && !isType()) return false;
	index++;

	if (!isTokenEqual(TOKEN_TYPE::identifier, {})) return false;
	funcName = curToken();
	index++;

	if (!isTokenEqual(TOKEN_TYPE::symbol, { "(" })) return false;

	symTabel.StartSubroutine();
	if (!compileParameterList()) return false;
	index++;

	if (!isTokenEqual(TOKEN_TYPE::symbol, { ")" })) return false;

	if (!compileSubroutineBody(fieldCount)) return false;
	symTabel.EndSubroutine();
	return true;
}

bool CompilationEngine::compileSubroutineBody(const int fieldCount) {
	index++;
	if (!isTokenEqual(TOKEN_TYPE::symbol, { "{" })) {
		index--;  return false;
	}
	int varCount = 0;
	while (compileVarDec(varCount));
	writer.writeFunction(className + "." + funcName, varCount);
	if (subroutineType == "method") {
		writer.writePush(SEGMENT::ARG, 0);
		writer.writePop(SEGMENT::POINTER, 0);
	}
	else if (subroutineType == "constructor") {
		writer.writePush(SEGMENT::CONST, fieldCount);
		writer.writeCall("Memory.alloc", 1);
		writer.writePop(SEGMENT::POINTER, 0);
	}

	if (!compileStatements()) return false;

	index++;
	if (!isTokenEqual(TOKEN_TYPE::symbol, { "}" })) return false;
	return true;
}

bool CompilationEngine::compileParameterList() {
	index++;

	if (!isType()) { index--; return true; }
	std::string symType = curToken();
	index++;

	if (!isTokenEqual(TOKEN_TYPE::identifier, {})) return false;
	symTabel.Define(curToken(), symType, VAR_KIND::ARG);
	index++;

	while (isTokenEqual(TOKEN_TYPE::symbol, { "," })) {
		index++;
		if (!isType()) return false;
		index++;
		if (!isTokenEqual(TOKEN_TYPE::identifier, {})) return false;
		symTabel.Define(curToken(), symType, VAR_KIND::ARG);
		index++;
	}
	index--;
	return true;
}

bool CompilationEngine::compileVarDec(int& varCount) {
	index++;
	if (!isTokenEqual(TOKEN_TYPE::keyword, { "var" })) {
		index--;  return false;
	}
	index++;

	if (!isType()) return false;
	std::string symType = curToken();
	index++;

	if (!isTokenEqual(TOKEN_TYPE::identifier, {})) return false;
	symTabel.Define(curToken(), symType, VAR_KIND::VAR);
	index++;
	varCount++;

	while (isTokenEqual(TOKEN_TYPE::symbol, { "," })) {
		index++;
		if (!isTokenEqual(TOKEN_TYPE::identifier, {})) return false;
		symTabel.Define(curToken(), symType, VAR_KIND::VAR);
		index++;
		varCount++;
	}

	if (!isTokenEqual(TOKEN_TYPE::symbol, { ";" })) return false;
	return true;
}

bool CompilationEngine::compileStatements() {
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
	return true;
}

bool CompilationEngine::compileDo() {
	index++;
	if (!isTokenEqual(TOKEN_TYPE::keyword, { "do" })) {
		index--; return false;
	}

	compileSubRoutineCall();
	writer.writePop(SEGMENT::TEMP, 0);

	index++;
	if (!isTokenEqual(TOKEN_TYPE::symbol, { ";" })) return false;
	return true;
}

bool CompilationEngine::compileSubRoutineCall() {
	index++;

	if (!isTokenEqual(TOKEN_TYPE::identifier, {})) return false;
	auto firstPart = curToken();
	index++;

	if (!isTokenEqual(TOKEN_TYPE::symbol, { "(", "." })) return false;

	int expCount = 0;
	if (curToken() == "(") {
		writer.writePush(SEGMENT::POINTER, 0);
		if (!compileExpressionList(expCount)) return false;
		index++;
		if (!isTokenEqual(TOKEN_TYPE::symbol, { ")" })) return false;
		writer.writeCall(className + "." + firstPart, expCount + 1);
	}
	else {
		if (symTabel.IndexOf(firstPart) != -1) {
			int symIndex = symTabel.IndexOf(firstPart);
			auto symKind = symTabel.KindOf(firstPart);
			switch (symKind)
			{
			case VAR_KIND::STATIC:
				writer.writePush(SEGMENT::STATIC, symIndex);
				break;
			case VAR_KIND::FIELD:
				writer.writePush(SEGMENT::THIS, symIndex);
				break;
			case VAR_KIND::ARG:
				if (subroutineType == "method")
					writer.writePush(SEGMENT::ARG, symIndex + 1);
				else
					writer.writePush(SEGMENT::ARG, symIndex);
				break;
			case VAR_KIND::VAR:
				writer.writePush(SEGMENT::LOCAL, symIndex);
				break;
			}
		}

		index++;
		if (!isTokenEqual(TOKEN_TYPE::identifier, {})) return false;
		auto secondPart = curToken();
		index++;

		if (!isTokenEqual(TOKEN_TYPE::symbol, { "(" })) return false;

		if (!compileExpressionList(expCount)) return false;

		if (symTabel.IndexOf(firstPart) != -1)
			writer.writeCall(symTabel.TypeOf(firstPart) + "." + secondPart, expCount + 1);
		else
			writer.writeCall(firstPart + "." + secondPart, expCount);

		index++;
		if (!isTokenEqual(TOKEN_TYPE::symbol, { ")" })) return false;
	}
}

bool CompilationEngine::compileLet() {
	index++;
	if (!isTokenEqual(TOKEN_TYPE::keyword, { "let" })) {
		index--; return false;
	}
	index++;

	if (!isTokenEqual(TOKEN_TYPE::identifier, {})) return false;
	std::string var = curToken();
	auto symIndex = symTabel.IndexOf(var);
	auto symKind = symTabel.KindOf(var);
	index++;

	bool useIndex = false;
	if (isTokenEqual(TOKEN_TYPE::symbol, { "[" })) {
		if (!compileExpression()) return false;
		index++;

		switch (symKind)
		{
		case VAR_KIND::STATIC:
			writer.writePush(SEGMENT::STATIC, symIndex);
			break;
		case VAR_KIND::FIELD:
			writer.writePush(SEGMENT::THIS, symIndex);
			break;
		case VAR_KIND::ARG:
			if (subroutineType == "method")
				writer.writePush(SEGMENT::ARG, symIndex + 1);
			else
				writer.writePush(SEGMENT::ARG, symIndex);
			break;
		case VAR_KIND::VAR:
			writer.writePush(SEGMENT::LOCAL, symIndex);
			break;
		}
		writer.writeArithmetic(ARITH_CMD::ADD);
		writer.writePop(SEGMENT::TEMP, 1);

		if (!isTokenEqual(TOKEN_TYPE::symbol, { "]" })) return false;
		index++;
		useIndex = true;
	}
	
	if (!isTokenEqual(TOKEN_TYPE::symbol, { "=" })) return false;

	if (!compileExpression()) return false;
	index++;

	if (useIndex) {
		writer.writePush(SEGMENT::TEMP, 1);
		writer.writePop(SEGMENT::POINTER, 1);
		writer.writePop(SEGMENT::THAT, 0);
	}
	else {
		switch (symKind)
		{
		case VAR_KIND::STATIC:
			writer.writePop(SEGMENT::STATIC, symIndex);
			break;
		case VAR_KIND::FIELD:
			writer.writePop(SEGMENT::THIS, symIndex);
			break;
		case VAR_KIND::ARG:
			if (subroutineType == "method")
				writer.writePop(SEGMENT::ARG, symIndex + 1);
			else
				writer.writePop(SEGMENT::ARG, symIndex);
			break;
		case VAR_KIND::VAR:
			writer.writePop(SEGMENT::LOCAL, symIndex);
			break;
		}
	}

	if (!isTokenEqual(TOKEN_TYPE::symbol, { ";" })) return false;
	return true;
}

bool CompilationEngine::compileWhile() {
	index++;
	if (!isTokenEqual(TOKEN_TYPE::keyword, { "while" })) {
		index--; return false;
	}
	index++;

	if (!isTokenEqual(TOKEN_TYPE::symbol, { "(" })) return false;

	auto label1 = "label_" + std::to_string(labelCount + 1);
	auto label2 = "label_" + std::to_string(labelCount + 2);
	labelCount += 2;
	writer.writeLabel(label1);
	if (!compileExpression()) return false;
	index++;
	writer.writeArithmetic(ARITH_CMD::NOT);
	writer.writeIf(label2);

	if (!isTokenEqual(TOKEN_TYPE::symbol, { ")" })) return false;
	index++;

	if (!isTokenEqual(TOKEN_TYPE::symbol, { "{" })) return false;

	if (!compileStatements()) return false;
	index++;

	writer.writeGoto(label1);
	writer.writeLabel(label2);

	if (!isTokenEqual(TOKEN_TYPE::symbol, { "}" })) return false;
	return true;
}

bool CompilationEngine::compileReturn() {
	index++;

	if (!isTokenEqual(TOKEN_TYPE::keyword, { "return" })) {
		index--; return false;
	}
	index++;

	if (curType() == TOKEN_TYPE::identifier ||
		isTokenEqual(TOKEN_TYPE::symbol, { "-", "~" }) ||
		curType() == TOKEN_TYPE::int_const ||
		curType() == TOKEN_TYPE::string_const ||
		isTokenEqual(TOKEN_TYPE::keyword, { "true", "false", "null", "this" })) {
		index--;
		if (!compileExpression()) return false;
		writer.writeReturn();
	}
	else {
		writer.writePush(SEGMENT::CONST, 0);
		writer.writeReturn();
		index--;
	}
	index++;
	
	if (!isTokenEqual(TOKEN_TYPE::symbol, { ";" })) return false;
	return true;
}

bool CompilationEngine::compileIf() {
	index++;
	if (!isTokenEqual(TOKEN_TYPE::keyword, { "if" })) {
		index--; return false;
	}
	index++;

	if (!isTokenEqual(TOKEN_TYPE::symbol, { "(" })) return false;

	if (!compileExpression()) return false;
	index++;

	if (!isTokenEqual(TOKEN_TYPE::symbol, { ")" })) return false;
	index++;

	auto label1 = "label_" + std::to_string(labelCount + 1);
	auto label2 = "label_" + std::to_string(labelCount + 2);
	labelCount += 2;
	writer.writeArithmetic(ARITH_CMD::NOT);
	writer.writeIf(label1);
	if (!isTokenEqual(TOKEN_TYPE::symbol, { "{" })) return false;

	if (!compileStatements()) return false;
	index++;
	writer.writeGoto(label2);
	writer.writeLabel(label1);
	if (!isTokenEqual(TOKEN_TYPE::symbol, { "}" })) return false;

	index++;
	if (isTokenEqual(TOKEN_TYPE::keyword, { "else" })) {
		index++;
		if (!isTokenEqual(TOKEN_TYPE::symbol, { "{" })) return false;

		if (!compileStatements()) return false;
		index++;

		if (!isTokenEqual(TOKEN_TYPE::symbol, { "}" })) return false;
	}
	else
		index--;
	writer.writeLabel(label2);
	return true;
}

bool CompilationEngine::compileExpression() {
	if (!compileTerm()) return false;
	index++;
	
	while (isTokenEqual(TOKEN_TYPE::symbol, { "+", "-", "*", "/", "&", "|", "<", ">", "=" })) {
		auto arith = curToken()[0];
		if (!compileTerm()) return false;
		index++;

		switch (arith) 
		{
		case '+':
			writer.writeArithmetic(ARITH_CMD::ADD);
			break;
		case '-':
			writer.writeArithmetic(ARITH_CMD::SUB);
			break;
		case '*':
			writer.writeCall("Math.multiply", 2);
			break;
		case '/':
			writer.writeCall("Math.divide", 2);
			break;
		case '&':
			writer.writeArithmetic(ARITH_CMD::AND);
			break;
		case '|':
			writer.writeArithmetic(ARITH_CMD::OR);
			break;
		case '<':
			writer.writeArithmetic(ARITH_CMD::LT);
			break;
		case '>':
			writer.writeArithmetic(ARITH_CMD::GT);
			break;
		case '=':
			writer.writeArithmetic(ARITH_CMD::EQ);
			break;
		}
	}
	index--;
	return true;
}

bool CompilationEngine::compileTerm() {
	index++;

	if (curType() == TOKEN_TYPE::int_const) {
		writer.writePush(SEGMENT::CONST, atoi(curToken().c_str()));
	}
	else if (curType() == TOKEN_TYPE::string_const) {
		writer.writePush(SEGMENT::CONST, curToken().size());
		writer.writeCall("String.new", 1);
		for (int i = 0; i < curToken().size(); i++) {
			writer.writePush(SEGMENT::CONST, curToken()[i]);
			writer.writeCall("String.appendChar", 2);
		}
	}
	else if (isTokenEqual(TOKEN_TYPE::keyword, { "true", "false", "null", "this" })) {
		if (curToken() == "null" || curToken() == "false")
			writer.writePush(SEGMENT::CONST, 0);
		if (curToken() == "true") {
			writer.writePush(SEGMENT::CONST, 1);
			writer.writeArithmetic(ARITH_CMD::NEG);
		}
		if (curToken() == "this")
			writer.writePush(SEGMENT::POINTER, 0);
	}
	else if (curType() == TOKEN_TYPE::identifier) {
		auto symIndex = symTabel.IndexOf(curToken());
		auto symKind = symTabel.KindOf(curToken());
		index++;
		if (isTokenEqual(TOKEN_TYPE::symbol, { "(" , "." })) {
			index -= 2;
			if (!compileSubRoutineCall()) return false;
		}
		else if (isTokenEqual(TOKEN_TYPE::symbol, { "[" })) {
			switch (symKind)
			{
			case VAR_KIND::STATIC:
				writer.writePush(SEGMENT::STATIC, symIndex);
				break;
			case VAR_KIND::FIELD:
				writer.writePush(SEGMENT::THIS, symIndex);
				break;
			case VAR_KIND::ARG:
				if (subroutineType == "method")
					writer.writePush(SEGMENT::ARG, symIndex + 1);
				else
					writer.writePush(SEGMENT::ARG, symIndex);
				break;
			case VAR_KIND::VAR:
				writer.writePush(SEGMENT::LOCAL, symIndex);
				break;
			}

			if (!compileExpression()) return false;
			index++;
			if (!isTokenEqual(TOKEN_TYPE::symbol, { "]" })) return false;

			writer.writeArithmetic(ARITH_CMD::ADD);
			writer.writePop(SEGMENT::POINTER, 1);
			writer.writePush(SEGMENT::THAT, 0);
		}
		else {
			switch (symKind)
			{
			case VAR_KIND::STATIC:
				writer.writePush(SEGMENT::STATIC, symIndex);
				break;
			case VAR_KIND::FIELD:
				writer.writePush(SEGMENT::THIS, symIndex);
				break;
			case VAR_KIND::ARG:
				if (subroutineType == "method")
					writer.writePush(SEGMENT::ARG, symIndex + 1);
				else
					writer.writePush(SEGMENT::ARG, symIndex);
				break;
			case VAR_KIND::VAR:
				writer.writePush(SEGMENT::LOCAL, symIndex);
				break;
			}
			index--;
		}
	}
	else if (isTokenEqual(TOKEN_TYPE::symbol, { "(" })) {
		if (!compileExpression()) return false;
		index++;

		if (!isTokenEqual(TOKEN_TYPE::symbol, { ")" })) return false;
	}
	else if (isTokenEqual(TOKEN_TYPE::symbol, { "-", "~" })) {
		auto arith = curToken();
		if (!compileTerm()) return false;
		if (arith == "-")
			writer.writeArithmetic(ARITH_CMD::NEG);
		else
			writer.writeArithmetic(ARITH_CMD::NOT);
	}
	else
		return false;
	return true;
}

bool CompilationEngine::compileExpressionList(int& expCount) {
	index++;
	if (curType() == TOKEN_TYPE::identifier ||
		isTokenEqual(TOKEN_TYPE::symbol, { "-", "~", "("}) ||
		curType() == TOKEN_TYPE::int_const ||
		curType() == TOKEN_TYPE::string_const ||
		isTokenEqual(TOKEN_TYPE::keyword, { "true", "false", "null", "this" })) {
		index--;
		if (!compileExpression()) return false;
		expCount++;
	}
	else {
		index--;
		return true;
	}

	index++;

	while (isTokenEqual(TOKEN_TYPE::symbol, { "," })) {
		if (!compileExpression()) return false;
		index++;
		expCount++;
	}
	index--;
	return true;
}

bool CompilationEngine::isType() {
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