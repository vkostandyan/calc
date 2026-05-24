#pragma once

#include "Compiler.hpp"
#include "CPU.hpp"
#include "Loader.hpp"

class Executor
{
public:
	Executor();
	~Executor();
	void loadAndRun(std::ifstream &exe);
	std::ifstream validate(const std::string &exePath);

private:
	Memory *memory;
	std::vector<int32_t> constPool;
	CPU *cpu;
};
