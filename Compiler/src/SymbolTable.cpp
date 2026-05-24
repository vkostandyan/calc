#include "../includes/SymbolTable.hpp"

SymbolTable::SymbolTable()
{
	scopes.push_back({});
}

SymbolTable::~SymbolTable() { }

size_t SymbolTable::getAddress(const std::string& name)
{
	if (scopes.empty())
		throw std::runtime_error("No active scope");
	for (int32_t i = scopes.size() - 1; i >= 0; --i)
	{
		auto it = scopes[i].find(name);
		if (it != scopes[i].end())
			return it->second;
	}
	throw std::runtime_error("Variable " + name + " is not declared");
}

int32_t SymbolTable::getValueByAddress(size_t address) const
{
	if (address < memory.size())
		return memory[address];
	throw std::runtime_error("Invalid address");
}

void SymbolTable::setVariable(const std::string& name, int32_t value)
{
	size_t addr = getAddress(name);
	memory[addr] = value;
}

bool SymbolTable::isDeclared(const std::string& name) const
{
	for (int32_t i = scopes.size() - 1; i >= 0; --i)
	{
		if (scopes[i].find(name) != scopes[i].end())
			return true;
	}
	return false;
}

void SymbolTable::declareVariable(const std::string& name)
{
	if (scopes.empty())
		throw std::runtime_error("No active scope");

	auto& current = scopes.back();

	if (isDeclared(name))
		throw std::runtime_error("Multiple definition of " + name);

	size_t addr = memory.size();
	current[name] = addr;
	memory.push_back(0);
}

void SymbolTable::setVariableByAddress(size_t address, int32_t value)
{
	if (address >= memory.size())
		throw std::runtime_error("Invalid address");

	memory[address] = value;
}

void SymbolTable::enterScope()
{
	scopes.push_back({});
}

void SymbolTable::exitScope()
{
	if (scopes.empty())
		throw std::runtime_error("No scope to exit");
	scopes.pop_back();
}