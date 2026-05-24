#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>

class SymbolTable
{
public:
	SymbolTable();
	~SymbolTable();
	size_t getAddress(const std::string&);
	void setVariable(const std::string&, int32_t);
	int32_t getValueByAddress(size_t) const;
	bool isDeclared(const std::string&) const;
	void declareVariable(const std::string&);
	void setVariableByAddress(size_t address, int32_t value);
	
	void enterScope();
	void exitScope();

private:
	std::vector<std::unordered_map<std::string, size_t>> scopes;
	std::vector<int32_t> memory;
};