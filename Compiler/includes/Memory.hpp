#pragma once

#include "Compiler.hpp"
#include <bitset>

static constexpr uint16_t MEM_SIZE       = 0xFFFF;

static constexpr uint16_t CODE_BASE      = 0x0000;
static constexpr uint16_t CODE_LIMIT     = 0x1FFF;

static constexpr uint16_t DATA_BASE      = 0x2000;
static constexpr uint16_t DATA_LIMIT     = 0x3FFF;

static constexpr uint16_t STACK_BASE     = 0xFFFE;
static constexpr uint16_t STACK_LIMIT    = 0x4000;

struct Instruction;

class Memory
{
public:
	Memory();

	int32_t read(uint16_t addr) const;
	void write(uint16_t addr, int32_t val);

	void writeCode(uint16_t codeAddr, const Instruction& inst);
	Instruction readCode(uint16_t codeAddr) const;
	int32_t readData(uint8_t varAddr) const;
	void writeData(uint8_t varAddr, int32_t val);

	void stackPush(uint16_t& sp, int32_t val);
	int32_t stackPop(uint16_t& sp);
	int32_t stackPeek(uint16_t sp) const;

	void dump(uint16_t from, uint16_t to) const;
private:
	std::vector<uint32_t> cells;
};
