#pragma once
#include <string>
#include <unordered_map>

class SymbolTable {
public:
	SymbolTable();

	void addEntry(const std::string& s, int num);
	bool contains(const std::string& symbol);
	int GetAddress(const std::string& symbol);
private:
	std::unordered_map<std::string, int> table;
};