#include "../includes/Memory.hpp"

Memory::Memory() : cells(MEM_SIZE, 0)
{
	std::cout << "[Memory] Initialised — " << MEM_SIZE
			<< " word-cells, all zeroed.\n";
	std::cout << "         Code  : 0x" << std::hex << CODE_BASE
			<< " - 0x" << CODE_LIMIT  << "\n";
	std::cout << "         Data  : 0x" << DATA_BASE
			<< " - 0x" << DATA_LIMIT  << "\n";
	std::cout << "         Stack : 0x" << STACK_LIMIT
			<< " - 0x" << STACK_BASE  << " (grows downward)\n"
			<< std::dec;
}


int32_t Memory::read(uint16_t addr) const
{
	return cells[addr];
}

void Memory::write(uint16_t addr, int32_t val)
{
	cells[addr] = val;
}

void Memory::writeCode(uint16_t codeAddr, const Instruction& inst)
{
	if (CODE_BASE + codeAddr > CODE_LIMIT)
		throw std::runtime_error("Memory::writeCode — code section overflow at index "
								+ std::to_string(codeAddr));

	int32_t packed =
		static_cast<uint8_t>(inst.op)
		| (static_cast<uint32_t>(inst.dest)  <<  8)
		| (static_cast<uint32_t>(inst.left)  << 16)
		| (static_cast<uint32_t>(inst.right) << 24);

	cells[CODE_BASE + codeAddr] = packed;
}

Instruction Memory::readCode(uint16_t codeAddr) const
{
	uint32_t packed = static_cast<uint32_t>(cells[CODE_BASE + codeAddr]);

	Instruction inst;
	inst.op    =  packed        & 0xFF;
	inst.dest  = (packed >>  8) & 0xFF;
	inst.left  = (packed >> 16) & 0xFF;
	inst.right = (packed >> 24) & 0xFF;
	return inst;
}


int32_t Memory::readData(uint8_t varAddr) const
{
	if (DATA_BASE + varAddr > DATA_LIMIT)
		throw std::runtime_error("Memory::readData — data address out of range: "
								+ std::to_string(varAddr));
	return cells[DATA_BASE + varAddr];
}

void Memory::writeData(uint8_t varAddr, int32_t val)
{
	if (DATA_BASE + varAddr > DATA_LIMIT)
		throw std::runtime_error("Memory::writeData — data address out of range: "
								+ std::to_string(varAddr));
	cells[DATA_BASE + varAddr] = val;
}

void Memory::stackPush(uint16_t& sp, int32_t val)
{
	if (sp <= STACK_LIMIT)
		throw std::runtime_error("Memory::stackPush — stack overflow (SP=0x"
								+ [&]{ std::ostringstream o; o << std::hex << sp; return o.str(); }()
								+ ")");
	cells[sp] = val;
	sp-= sizeof(uint32_t);
	std::cout << "[Stack] PUSH " << "  →  SP now 0x" << std::hex << sp << std::dec << "\n";
}

int32_t Memory::stackPop(uint16_t& sp)
{
	if (sp >= STACK_BASE)
		throw std::runtime_error("Memory::stackPop — stack underflow (SP=0x"
								+ [&]{ std::ostringstream o; o << std::hex << sp; return o.str(); }()
								+ ")");
	sp+= sizeof(uint32_t);
	int32_t val = cells[sp];
	std::cout << "[Stack] POP  " << "  ←  SP now 0x" << std::hex << sp << std::dec << "\n";
	return val;
}

int32_t Memory::stackPeek(uint16_t sp) const
{
	if (sp >= STACK_BASE)
		throw std::runtime_error("Memory::stackPeek — stack is empty");
	return cells[sp + 1];
}


void Memory::dump(uint16_t from, uint16_t to) const
{
	std::cout << "\n[Memory dump  0x"
			<< std::hex << std::setw(4) << std::setfill('0') << from
			<< " - 0x"
			<< std::setw(4) << std::setfill('0') << to
			<< "]\n" << std::dec;

	bool anyNonZero = false;
	for (uint32_t a = from; a <= to; ++a)
	{
		if (cells[a] != 0)
		{
			std::cout << "  [0x" << std::hex << std::setw(4) << std::setfill('0')
					<< a << "] = " << std::bitset<32>(cells[a]) << "\n";
			anyNonZero = true;
		}
	}
	if (!anyNonZero)
		std::cout << "  (all zero)\n";
}