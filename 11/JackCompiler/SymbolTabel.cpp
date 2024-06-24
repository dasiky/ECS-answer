#include "SymbolTabel.h"

void SymbolTabel::StartSubroutine() {
	tabels.emplace_back();
}

void SymbolTabel::EndSubroutine() {
	tabels.pop_back();
}

void SymbolTabel::Define(const std::string& name, const std::string& type, VAR_KIND kind) {
	auto& tabel = tabels.back();
	tabel[name] = Var{ kind, type, VarCount(kind)};
}

int SymbolTabel::VarCount(VAR_KIND kind) {
	int res = 0;
	switch (kind)
	{
	case VAR_KIND::STATIC:
	case VAR_KIND::FIELD:
		for (auto sym : tabels[0])
			if (sym.second.kind == kind)
				res++;
		break;
	case VAR_KIND::ARG:
	case VAR_KIND::VAR:
		for (auto tabel : tabels)
			for (auto sym : tabel)
				if (sym.second.kind == kind)
					res++;
		break;
	}
	return res;
}

VAR_KIND SymbolTabel::KindOf(const std::string& name) {
	for (auto iter = tabels.rbegin(); iter != tabels.rend(); iter++)
		if (iter->count(name))
			return (*iter)[name].kind;
	return VAR_KIND::NONE;
}

std::string SymbolTabel::TypeOf(const std::string& name) {
	for (auto iter = tabels.rbegin(); iter != tabels.rend(); iter++)
		if (iter->count(name))
			return (*iter)[name].type;
	return "";
}

int SymbolTabel::IndexOf(const std::string& name) {
	for (auto iter = tabels.rbegin(); iter != tabels.rend(); iter++)
		if (iter->count(name))
			return (*iter)[name].index;
	return -1;
}
