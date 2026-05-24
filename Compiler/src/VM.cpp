#include "../includes/VM.hpp"

VM::VM() : regs(100), next(4), cmpFlag(0) {}

VM::~VM() {}

int32_t VM::compileNum(Node *node)
{
	int32_t dest = next++;

	uint8_t cIdx = static_cast<uint8_t>(constants.size());
	constants.push_back(node->value);

	program.push_back({static_cast<uint8_t>(OpCode::LOAD_NUM),
					static_cast<uint8_t>(dest),
					cIdx,
					0});

	return dest;
}

int32_t VM::compileVar(Node *node)
{
	int32_t dest = next++;

	program.push_back({static_cast<uint8_t>(OpCode::LOAD_VAR),
					static_cast<uint8_t>(dest),
					static_cast<uint8_t>(node->symAddr),
					0});

	return dest;
}

int32_t VM::compileOp(Node *node)
{
	int32_t l = compile(node->left.get());
	int32_t r = compile(node->right.get());

	int32_t dest = next++;

	OpCode op;
	switch (node->op)
	{
	case Operator::Add:
		op = OpCode::ADD;
		break;
	case Operator::Sub:
		op = OpCode::SUB;
		break;
	case Operator::Mult:
		op = OpCode::MUL;
		break;
	case Operator::Div:
		op = OpCode::DIV;
		break;
	default:
		throw std::runtime_error("Unknown operator");
	}

	program.push_back({static_cast<uint8_t>(op),
					static_cast<uint8_t>(dest),
					static_cast<uint8_t>(l),
					static_cast<uint8_t>(r)});

	return dest;
}

int32_t VM::compileAssign(Node *node)
{
	size_t addr = node->left->symAddr;

	int32_t rhs = compile(node->right.get());

	program.push_back({static_cast<uint8_t>(OpCode::STORE_VAR),
					static_cast<uint8_t>(rhs),
					static_cast<uint8_t>(addr),
					0});

	return rhs;
}

int32_t VM::compileBlock(Node *node)
{
	const BlockNode *bnode = static_cast<const BlockNode *>(node);
	for (auto &stmt : bnode->statements)
		compile(stmt.get());

	return -1;
}

int32_t VM::compileIf(Node *node)
{
	IfNode *n = dynamic_cast<IfNode *>(node);

	int32_t jmpIndex = compileComp(n->condition.get());

	compile(n->trueBranch.get());

	if (n->falseBranch)
	{
		int32_t jmpEnd = program.size();
		program.push_back({static_cast<uint8_t>(OpCode::JMP),
						0, 0, 0});

		program[jmpIndex].dest = static_cast<uint8_t>(program.size());

		compile(n->falseBranch.get());

		program[jmpEnd].dest = static_cast<uint8_t>(program.size());
	}
	else
		program[jmpIndex].dest = static_cast<uint8_t>(program.size());

	return -1;
}

int32_t VM::compileWhile(Node *node)
{
	WhileNode *n = dynamic_cast<WhileNode *>(node);

	int32_t loopStart = static_cast<int32_t>(program.size());

	int32_t jmpExit = compileComp(n->condition.get());

	compile(n->body.get());

	program.push_back({static_cast<uint8_t>(OpCode::JMP),
					static_cast<uint8_t>(loopStart),
					0, 0});

	program[jmpExit].dest = static_cast<uint8_t>(program.size());

	return -1;
}

int32_t VM::compileComp(Node *node)
{
	int32_t l = compile(node->left.get());
	int32_t r = compile(node->right.get());

	program.push_back({static_cast<uint8_t>(OpCode::CMP),
					0,
					static_cast<uint8_t>(l),
					static_cast<uint8_t>(r)});

	OpCode jmp;
	if (node->name == "==")
		jmp = OpCode::JNE;
	else if (node->name == "!=")
		jmp = OpCode::JE;
	else if (node->name == ">=")
		jmp = OpCode::JL;
	else if (node->name == "<=")
		jmp = OpCode::JG;
	else if (node->name == ">")
		jmp = OpCode::JLE;
	else if (node->name == "<")
		jmp = OpCode::JGE;
	else
		throw std::runtime_error("Unknown comparison operator: " + node->name);

	int32_t jmpIndex = static_cast<int32_t>(program.size());

	program.push_back({static_cast<uint8_t>(jmp),
					0, 0, 0});

	return jmpIndex;
}

void VM::compileFunction(Node *node)
{
	FuncNode *fn = static_cast<FuncNode *>(node);
	functions[fn->name] = static_cast<int32_t>(program.size());
	for (int i = fn->params.size() - 1; i >= 0; --i)
	{
		int32_t reg = next++;

		program.push_back({static_cast<uint8_t>(OpCode::POP),
						static_cast<uint8_t>(reg),
						0,
						0});

		program.push_back({static_cast<uint8_t>(OpCode::STORE_VAR),
						static_cast<uint8_t>(reg),
						static_cast<uint8_t>(fn->params[i].symAddr),
						0});
	}
	int32_t savedNext = next;

	compile(fn->body.get());

	if (program.empty() ||
		static_cast<OpCode>(program.back().op) != OpCode::RET)
	{
		program.push_back({static_cast<uint8_t>(OpCode::RET),
						0, 0, 0});
	}

	next = savedNext;
}

int32_t VM::compileCall(Node *node)
{
	CallNode *cnode = static_cast<CallNode *>(node);

	for (auto it = cnode->args.rbegin();
		it != cnode->args.rend();
		++it)
	{
		int32_t reg = compile(it->get());

		program.push_back({static_cast<uint8_t>(OpCode::PUSH),
						static_cast<uint8_t>(reg),
						0,
						0});
	}

	auto it = functions.find(cnode->name);
	if (it == functions.end())
		throw std::runtime_error("Undefined function: " + cnode->name);

	int32_t dest = next++;

	program.push_back({static_cast<uint8_t>(OpCode::CALL),
					static_cast<uint8_t>(dest),
					static_cast<uint8_t>(it->second),
					0});

	return dest;
}

int32_t VM::compileReturn(Node *node)
{
	ReturnNode *ret = static_cast<ReturnNode *>(node);

	if (ret->expr)
	{
		int32_t reg = compile(ret->expr.get());

		program.push_back({static_cast<uint8_t>(OpCode::RET),
						static_cast<uint8_t>(reg),
						0, 0});

		return reg;
	}

	program.push_back({static_cast<uint8_t>(OpCode::RET),
					0, 0, 0});

	return -1;
}

int32_t VM::compile(Node *node)
{
	if (!node)
		return -1;

	switch (node->type)
	{
	case NodeType::Num:
		return compileNum(node);
	case NodeType::Var:
		return compileVar(node);
	case NodeType::Op:
		return compileOp(node);
	case NodeType::Assign:
		return compileAssign(node);
	case NodeType::Block:
		return compileBlock(node);
	case NodeType::If:
		return compileIf(node);
	case NodeType::Comp:
		return compileComp(node);
	case NodeType::While:
		return compileWhile(node);
	case NodeType::Ret:
		return compileReturn(node);
	case NodeType::Func:
		compileFunction(static_cast<FuncNode *>(node));
		return -1;
	case NodeType::Call:
		return compileCall(static_cast<CallNode *>(node));
	default:
		throw std::runtime_error("Unknown node type in compile: " + std::to_string(static_cast<int32_t>(node->type)));
	}
}

void VM::writeInExe()
{
	std::fstream exe("./exe", std::ios::out | std::ios::trunc);
	if (!exe.is_open())
		throw std::runtime_error("Could not open ./exe");

	std::stringstream symtblSection;

	symtblSection << ".SymblTbl\n";
	uint32_t offset = 8;

	for (const auto &[name, addr] : functions)
		symtblSection << name << " " << addr << "\n";

	std::string symtblStr = symtblSection.str();

	std::stringstream codeSection;
	writeInExeCode(codeSection);
	std::string codeStr = codeSection.str();

	std::vector<SectionInfo> sections;
	std::stringstream headerStream;

	headerStream << "~AnushFile\n";
	headerStream << "Header Size: 2\n\n";
	headerStream << "Name\t\tSize\tOffset\n";

	sections.push_back({".SymblTbl",
						static_cast<uint32_t>(functions.size()),
						0});

	sections.push_back({".CODE",
						static_cast<uint32_t>(std::count(codeStr.begin(), codeStr.end(), '\n') - 2),
						0});

	for (const auto &sec : sections)
	{
		headerStream
			<< sec.name << "\t"
			<< sec.size << "\t"
			<< "0000\n";
	}
	headerStream << "\n";

	sections[0].offset = offset;
	sections[1].offset = sections[0].offset + sections[0].size + 2;

	std::stringstream finalHeader;

	finalHeader << "~AnushFile\n";
	finalHeader << "Header Size: 2\n\n";
	finalHeader << "Name\t\tSize\tOffset\n";

	for (const auto &sec : sections)
	{
		finalHeader
			<< sec.name << "\t"
			<< sec.size << "\t"
			<< sec.offset << "\n";
	}

	finalHeader << "\n";

	exe << finalHeader.str();

	exe << symtblStr;
	exe << codeStr;

	exe.close();
}

void VM::writeInExeCode(std::ostream &exe)
{
	exe << "\n.CODE\n";

	auto it = functions.begin();
	while (it != functions.end())
	{
		if (it->first == "main")
			break;
		it++;
	}
	if (it != functions.end())
		exe << "CALL " << it->second << std::endl;
	exe << "EXIT"<< std::endl;

	for (size_t i = 0; i < program.size(); i++)
	{
		const auto &inst = program[i];

		switch (static_cast<OpCode>(inst.op))
		{
		case OpCode::LOAD_NUM:
			exe << "MOV r" << static_cast<int32_t>(inst.dest)
				<< ", #" << constants[inst.left] << std::endl;
			break;
		case OpCode::LOAD_VAR:
			exe << "MOV r" << static_cast<int32_t>(inst.dest)
				<< ", [" << static_cast<int32_t>(inst.left) << "]" << std::endl;
			break;
		case OpCode::STORE_VAR:
			exe << "MOV [" << static_cast<int32_t>(inst.left)
				<< "], r" << static_cast<int32_t>(inst.dest) << std::endl;
			break;
		case OpCode::ADD:
			exe << "ADD r" << static_cast<int32_t>(inst.dest)
				<< ", r" << static_cast<int32_t>(inst.left)
				<< ", r" << static_cast<int32_t>(inst.right) << std::endl;
			break;
		case OpCode::SUB:
			exe << "SUB r" << static_cast<int32_t>(inst.dest)
				<< ", r" << static_cast<int32_t>(inst.left)
				<< ", r" << static_cast<int32_t>(inst.right) << std::endl;
			break;
		case OpCode::MUL:
			exe << "MUL r" << static_cast<int32_t>(inst.dest)
				<< ", r" << static_cast<int32_t>(inst.left)
				<< ", r" << static_cast<int32_t>(inst.right) << std::endl;
			break;
		case OpCode::DIV:
			exe << "DIV r" << static_cast<int32_t>(inst.dest)
				<< ", r" << static_cast<int32_t>(inst.left)
				<< ", r" << static_cast<int32_t>(inst.right) << std::endl;
			break;
		case OpCode::CMP:
			exe << "CMP r" << static_cast<int32_t>(inst.left)
				<< ", r" << static_cast<int32_t>(inst.right) << std::endl;
			break;
		case OpCode::JMP:
			exe << "JMP " << static_cast<int32_t>(inst.dest) << std::endl;
			break;
		case OpCode::JE:
			exe << "JE " << static_cast<int32_t>(inst.dest) << std::endl;
			break;
		case OpCode::JNE:
			exe << "JNE " << static_cast<int32_t>(inst.dest) << std::endl;
			break;
		case OpCode::JG:
			exe << "JG " << static_cast<int32_t>(inst.dest) << std::endl;
			break;
		case OpCode::JL:
			exe << "JL " << static_cast<int32_t>(inst.dest) << std::endl;
			break;
		case OpCode::JGE:
			exe << "JGE " << static_cast<int32_t>(inst.dest) << std::endl;
			break;
		case OpCode::JLE:
			exe << "JLE " << static_cast<int32_t>(inst.dest) << std::endl;
			break;
		case OpCode::CALL:
			exe << "CALL " << static_cast<int32_t>(inst.left) << std::endl;
			break;
		case OpCode::RET:
			exe << "RET";
			if (inst.dest != 0)
				exe << " r" << static_cast<int32_t>(inst.dest);
			exe << std::endl;
			break;
		case OpCode::PUSH:
			exe << "PUSH r" << static_cast<int32_t>(inst.dest) << std::endl;
			break;
		case OpCode::POP:
			exe << "POP r" << static_cast<int32_t>(inst.dest) << std::endl;
			break;
		default:
			throw std::runtime_error("Undefined OpCode");
		}
	}
}
