#pragma once

#include "Compiler.hpp"
#include <algorithm>
struct Instruction;

enum class Operand
{
	MEM,
	REG,
	CONST,
};

struct SectionInfo
{
	std::string name;
	uint32_t size;
	uint32_t offset;
};

class VM
{
private:
	std::vector<int32_t> regs;
	int32_t next;
	std::vector<Instruction> program;
	std::unordered_map<std::string, int32_t> functions;
	std::vector<int32_t> constants;
	int32_t cmpFlag;

	int32_t compileNum(Node *node);
	int32_t compileVar(Node *node);
	int32_t compileOp(Node *node);
	int32_t compileAssign(Node *node);
	int32_t compileBlock(Node *node);
	int32_t compileIf(Node *node);
	int32_t compileComp(Node *node);
	int32_t compileWhile(Node *node);
	void compileFunction(Node *node);
	int32_t compileReturn(Node *node);
	int32_t compileCall(Node *node);

	void writeInExeCode(std::ostream &exe);

public:
	VM();
	~VM();
	void visualize() const;
	int32_t compile(Node *node);
	void writeInExe();
};
