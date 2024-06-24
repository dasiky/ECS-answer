#pragma once
#include <string>
#include <vector>
#include <unordered_map>

enum class VAR_KIND {
	STATIC,
	FIELD,
	ARG,
	VAR,
	NONE
};

class SymbolTabel
{
public:
	void StartSubroutine();
	void EndSubroutine();
	void Define(const std::string& name, const std::string& type, VAR_KIND kind);
	int VarCount(VAR_KIND kind);
	VAR_KIND KindOf(const std::string& name);
	std::string TypeOf(const std::string& name);
	int IndexOf(const std::string& name);
private:
	struct Var
	{
		VAR_KIND kind;
		std::string type;
		int index;
	};

	std::vector<std::unordered_map<std::string, Var>> tabels;
};