#include "../includes/Loader.hpp"

Instruction Loader::parseLine(const std::string& raw, std::vector<int32_t>& constPool)
{
	std::istringstream ss(raw);
	std::string cmd;
	ss >> cmd;
	if (cmd.empty())
		return { static_cast<uint8_t>(OpCode::EXIT), 0, 0, 0 };

	auto stripComma = [](std::string& s) {
		if (!s.empty() && s.back() == ',') s.pop_back();
	};

	if (cmd == "MOV")
	{
		std::string dst, src;
		ss >> dst >> src;
		stripComma(dst);

		if (dst[0] == 'r')
		{
			uint8_t D = static_cast<uint8_t>(std::stoi(dst.substr(1)));

			if (src[0] == '#')   // MOV rN, #literal  → LOAD_NUM
			{
				int32_t val = std::stoi(src.substr(1));
				uint8_t idx = static_cast<uint8_t>(constPool.size());
				constPool.push_back(val);
				return { static_cast<uint8_t>(OpCode::LOAD_NUM), D, idx, 0 };
			}
			else if (src[0] == '[')  // MOV rN, [addr]  → LOAD_VAR
			{
				std::string addrStr = src.substr(1);
				if (!addrStr.empty() && addrStr.back() == ']') addrStr.pop_back();
				uint8_t addr = static_cast<uint8_t>(std::stoi(addrStr));
				return { static_cast<uint8_t>(OpCode::LOAD_VAR), D, addr, 0 };
			}
		}
		else if (dst[0] == '[')
		{
			std::string addrStr = dst.substr(1);
			if (!addrStr.empty() && addrStr.back() == ']') addrStr.pop_back();
			uint8_t addr = static_cast<uint8_t>(std::stoi(addrStr));
			std::string srcReg = src;
			stripComma(srcReg);
			uint8_t S = static_cast<uint8_t>(std::stoi(srcReg.substr(1)));
			return { static_cast<uint8_t>(OpCode::STORE_VAR), S, addr, 0 };
		}
		throw std::runtime_error("Loader: unrecognised MOV form: " + raw);
	}

	{
		OpCode arith = static_cast<OpCode>(0xFF);
		if      (cmd == "ADD") arith = OpCode::ADD;
		else if (cmd == "SUB") arith = OpCode::SUB;
		else if (cmd == "MUL") arith = OpCode::MUL;
		else if (cmd == "DIV") arith = OpCode::DIV;

		if (arith != static_cast<OpCode>(0xFF))
		{
			std::string rD, rL, rR;
			ss >> rD >> rL >> rR;
			stripComma(rD); stripComma(rL);
			uint8_t D = static_cast<uint8_t>(std::stoi(rD.substr(1)));
			uint8_t L = static_cast<uint8_t>(std::stoi(rL.substr(1)));
			uint8_t R = static_cast<uint8_t>(std::stoi(rR.substr(1)));
			return { static_cast<uint8_t>(arith), D, L, R };
		}
	}

	if (cmd == "CMP")
	{
		std::string rL, rR;
		ss >> rL >> rR;
		if (!rL.empty() && rL.back() == ',') rL.pop_back();
		uint8_t L = static_cast<uint8_t>(std::stoi(rL.substr(1)));
		uint8_t R = static_cast<uint8_t>(std::stoi(rR.substr(1)));
		return { static_cast<uint8_t>(OpCode::CMP), 0, L, R };
	}

	{
		OpCode jop = static_cast<OpCode>(0xFF);
		if      (cmd == "JMP") jop = OpCode::JMP;
		else if (cmd == "JE")  jop = OpCode::JE;
		else if (cmd == "JNE") jop = OpCode::JNE;
		else if (cmd == "JG")  jop = OpCode::JG;
		else if (cmd == "JL")  jop = OpCode::JL;
		else if (cmd == "JGE") jop = OpCode::JGE;
		else if (cmd == "JLE") jop = OpCode::JLE;

		if (jop != static_cast<OpCode>(0xFF))
		{
			std::string addr;
			ss >> addr;
			uint8_t target = static_cast<uint8_t>(std::stoi(addr));
			return { static_cast<uint8_t>(jop), target, 0, 0 };
		}
	}

	if (cmd == "CALL")
	{
		std::string addr;
		ss >> addr;
		uint8_t target = static_cast<uint8_t>(std::stoi(addr));
		return { static_cast<uint8_t>(OpCode::CALL), 0, target, 0 };
	}

	if (cmd == "RET")
	{
		std::string rv;
		ss >> rv;
		uint8_t retReg = 0;
		if (!rv.empty() && rv[0] == 'r')
			retReg = static_cast<uint8_t>(std::stoi(rv.substr(1)));
		return { static_cast<uint8_t>(OpCode::RET), retReg, 0, 0 };
	}

	if (cmd == "PUSH")
	{
		std::string r;
		ss >> r;
		uint8_t R = static_cast<uint8_t>(std::stoi(r.substr(1)));
		return { static_cast<uint8_t>(OpCode::PUSH), R, 0, 0 };
	}

	if (cmd == "POP")
	{
		std::string r;
		ss >> r;
		uint8_t R = static_cast<uint8_t>(std::stoi(r.substr(1)));
		return { static_cast<uint8_t>(OpCode::POP), R, 0, 0 };
	}

	if (cmd == "EXIT")
		return { static_cast<uint8_t>(OpCode::EXIT), 0, 0, 0 };

	throw std::runtime_error("Loader: unrecognised instruction: '" + raw + "'");
}


uint16_t Loader::loadFromStream(std::istream& stream, Memory* mem, std::vector<int32_t>& constPool)
{
	std::string line;
	bool inCode = false;
	std::vector<std::string> codeLines;

	while (std::getline(stream, line))
	{
		if (line == ".CODE")
		{
			inCode = true;
			continue;
		}
		if (inCode)
		{
			if (!line.empty())
				codeLines.push_back(line);
		}
	}

	if (!inCode)
		throw std::runtime_error("Loader: .CODE section not found in executable");

	uint16_t addr = 0;
	auto code_it = codeLines.begin();
	while (code_it != codeLines.end())
	{
		if (addr > CODE_LIMIT - CODE_BASE)
			throw std::runtime_error("Loader: code section overflow");

		Instruction inst = parseLine(*code_it, constPool);
		mem->writeCode(addr, inst);

		std::cout << "[Loader] [" << std::hex << std::setw(4) << std::setfill('0') << addr << "] loaded: "
					<< *code_it << "\n";
		code_it++;
		addr += sizeof(Instruction);
	}

	std::cout << "[Loader] Load complete — "
			<< addr << " instruction(s) written, "
			<< constPool.size() << " constant(s) in pool.\n";

	return addr;
}


uint16_t Loader::load(const std::string& path, Memory* mem, std::vector<int32_t>& constPool)
{
	std::ifstream f(path);
	if (!f.is_open())
		throw std::runtime_error("Loader: cannot open '" + path + "'");


	std::string magic;
	std::getline(f, magic);
	if (magic != "~AnushFile")
		throw std::runtime_error("Loader: bad magic in '" + path + "'");

	std::cout << "[Loader] File '" << path << "' opened and magic verified.\n";
	return loadFromStream(f, mem, constPool);
}