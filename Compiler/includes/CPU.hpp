#pragma once

#include "Compiler.hpp"

static constexpr int NUM_REGS = 16;

static constexpr uint8_t REG_ZERO = 0;   // always 0
static constexpr uint8_t REG_RA   = 1;   // return address
static constexpr uint8_t REG_SP   = 2;   // stack pointer
static constexpr uint8_t REG_FP   = 3;   // frame pointer

class CPU
{
public:
	CPU();
	~CPU() = default;

	void run(Memory* mem);
	void dumpRegisters() const;
	void setConstPool(const std::vector<int32_t>& _constPool);
private:
	int32_t regs[NUM_REGS];   // x0–x31  (x0 always 0)
	uint16_t IP;               // instruction pointer (indexes into code section)
	uint16_t SP;               // stack pointer
	int32_t  cmpFlag;          // result of last CMP
	std::vector<int32_t> constPool;

	int32_t reg(uint8_t r)const;
	void setReg(uint8_t r, int32_t v);
	void push(int32_t val, Memory* mem);
	int32_t pop(Memory* mem);

	Instruction fetch(const Memory *mem);
	uint8_t decode(const Instruction& inst);
	void execute(const Instruction& inst, const uint8_t op, Memory* mem);
};
