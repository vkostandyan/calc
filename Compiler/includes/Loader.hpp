#pragma once

#include "Compiler.hpp"

class Loader
{
public:
	static uint16_t load(const std::string &path, Memory *mem, std::vector<int32_t> &constPool);
	static uint16_t loadFromStream(std::istream &stream, Memory *mem, std::vector<int32_t> &constPool);

private:
	static Instruction parseLine(const std::string &raw, std::vector<int32_t> &constPool);
};