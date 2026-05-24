#include "../includes/Executor.hpp"
#include "../includes/CPU.hpp"

Executor::Executor() {}

Executor::~Executor()
{
	delete memory;
	delete cpu;
}

std::ifstream Executor::validate(const std::string &exePath)
{
	std::ifstream f(exePath);
	if (!f.is_open())
		throw std::runtime_error("Cannot open '" + exePath + "'");

	std::string line;
	std::getline(f, line);
	if (line != "~AnushFile")
		throw std::runtime_error("Invalid file '" + exePath + "'");
	std::cout << "[Executor] Settng up Execution Environment\n";
	memory = new Memory();
	cpu = new CPU();
	return f;
}


void Executor::loadAndRun(std::ifstream& exe)
{
    std::cout << "\n[Executor] Loading program...\n";

    uint16_t nInstructions = Loader::loadFromStream(exe, memory, constPool);
 
    std::cout << "[Executor] Loaded " << nInstructions
              << " instruction(s)\n";
 
    memory->dump(CODE_BASE, CODE_BASE + nInstructions);

    std::cout << "\n[Executor] Starting CPU...\n";
 
    cpu->setConstPool(constPool);
    cpu->run(memory);
 
    std::cout << "\n[Executor] Execution complete.\n";
    cpu->dumpRegisters();

}
