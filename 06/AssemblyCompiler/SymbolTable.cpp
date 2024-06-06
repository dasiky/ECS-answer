#include "SymbolTable.h"

SymbolTable::SymbolTable() {
	table = std::unordered_map<std::string, int>{
		{"SP", 0},
		{"LCL", 1},
		{"ARG", 2},
		{"THIS", 3},
		{"THAT", 4},
		{"SCREEN", 16384},
		{"KBD", 24576}
	};

	for (int i = 0; i <= 15; i++) {
		table["R" + std::to_string(i)] = i;
	}
}

void SymbolTable::addEntry(const std::string& s, int num) {
	table[s] = num;
}

bool SymbolTable::contains(const std::string& symbol) {
	return table.count(symbol);
}

int SymbolTable::GetAddress(const std::string& symbol) {
	if (contains(symbol))
		return table[symbol];
	return 0;
}
