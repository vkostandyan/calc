#include "../includes/CPU.hpp"

CPU::CPU(): IP(0), SP(STACK_BASE), cmpFlag(0)
{
	for (int i = 0; i < NUM_REGS; ++i)
		regs[i] = 0;
	regs[REG_SP] = STACK_BASE;
	std::cout << "[CPU] Initialised — "
			<< NUM_REGS << " registers, IP=0, SP=0x"
			<< std::hex << STACK_BASE << std::dec << "\n";

}

void CPU::setConstPool(const std::vector<int32_t>& _constPool)
{
	constPool = _constPool;
}

int32_t CPU::reg(uint8_t r) const
{
	return r == 0 ? 0 : regs[r];
}

void CPU::setReg(uint8_t r, int32_t v)
{
	if (r != 0)
		regs[r] = v;
}

void CPU::push(int32_t val, Memory* mem)
{
	mem->stackPush(SP, val);
}

int32_t CPU::pop(Memory* mem)
{
	return mem->stackPop(SP);
}

Instruction CPU::fetch(const Memory *mem)
{
	return mem->readCode(IP * sizeof(Instruction));
}

uint8_t CPU::decode(const Instruction& inst)
{
	return (inst.op);
}


void CPU::execute(const Instruction& inst, const uint8_t op, Memory* mem)
{
	switch (static_cast<OpCode>(op)) {

	case OpCode::LOAD_NUM:
		setReg(inst.dest, constPool.at(inst.left));
		break;

	case OpCode::LOAD_VAR:
		setReg(inst.dest, mem->readData(inst.left));
		break;

	case OpCode::STORE_VAR:
		mem->writeData(inst.left, reg(inst.dest));
		break;

	case OpCode::ADD:
		setReg(inst.dest, reg(inst.left) + reg(inst.right));
		break;

	case OpCode::SUB:
		setReg(inst.dest, reg(inst.left) - reg(inst.right));
		break;

	case OpCode::MUL:
		setReg(inst.dest, reg(inst.left) * reg(inst.right));
		break;

	case OpCode::DIV:
		if (reg(inst.right) == 0)
			throw std::runtime_error("CPU: division by zero");
		setReg(inst.dest, reg(inst.left) / reg(inst.right));
		break;

	case OpCode::CMP:
		cmpFlag = reg(inst.left) - reg(inst.right);
		break;

	case OpCode::JMP:
		IP = 2 + inst.dest;
		break;

	case OpCode::JE:
		if (cmpFlag == 0) IP = 2 + inst.dest;
		break;

	case OpCode::JNE:
		if (cmpFlag != 0) IP = 2 + inst.dest;
		break;

	case OpCode::JG:
		if (cmpFlag > 0) IP = 2 + inst.dest;
		break;

	case OpCode::JL:
		if (cmpFlag < 0) IP = 2 + inst.dest;
		break;

	case OpCode::JGE:
		if (cmpFlag >= 0) IP = 2 + inst.dest;
		break;

	case OpCode::JLE:
		if (cmpFlag <= 0) IP = 2 + inst.dest;
		break;

	case OpCode::CALL:
		// printf("CALL\n");
		push(IP+1, mem);
		push(reg(REG_FP), mem);
		setReg(REG_FP, SP);
		setReg(REG_RA, IP+1);
		IP = 2 + inst.left;
		break;

	case OpCode::RET:
		setReg(REG_FP, pop(mem)); // restore previous FP
	    IP = pop(mem);
		break;

	case OpCode::PUSH:
		push(reg(inst.dest), mem);
		break;

	case OpCode::POP:
		setReg(inst.dest, pop(mem));
		break;
	
	default:
		throw std::runtime_error("Undefined EXIT");
	}
}

void CPU::run(Memory* mem)
{
	while (true) {
		Instruction inst = fetch(mem);
		uint8_t opcode = decode(inst);
		if (static_cast<OpCode>(opcode) == OpCode::EXIT)
			return;
		uint16_t oldIP = IP;

		execute(inst, inst.op, mem);

		if (IP == oldIP)
			IP++;
	}
}

void CPU::dumpRegisters() const
{
	std::cout << "\n[Register file]\n";
	const char* names[] = {
		"zero","ra","sp","fp",
		"t0","t1","t2","t3","t4","t5","t6","t7",
		"s0","s1","s2","s3","s4","s5","s6","s7",
		"a0","a1","a2","a3","a4","a5","a6","a7",
		"x28","x29","x30","x31"
	};
	for (int i = 0; i < NUM_REGS; ++i) {
		int32_t v = (i == 0) ? 0 : regs[i];
		if (v != 0)
			std::cout << "  x" << std::setw(2) << std::left << i
						<< " (" << std::setw(4) << names[i] << ") = "
						<< v << "\n";
	}
	std::cout << "  IP = " << IP
				<< "   SP = 0x" << std::hex << SP << std::dec
				<< "   cmpFlag = " << cmpFlag << "\n";
}
