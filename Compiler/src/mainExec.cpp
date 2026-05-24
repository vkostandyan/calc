#include "../includes/Compiler.hpp"
#include "../includes/Executor.hpp"

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <exe-file>\n";
		return 1;
	}

	std::string exePath = argv[1];

	Executor exec;
	try
	{
		std::ifstream file = exec.validate(exePath);
		
		exec.loadAndRun(file);
	}
	catch (const std::exception &e)
	{
		std::cerr << "[EXECUTION ERROR] " << e.what() << "\n";
		return 1;
	}

	return 0;
}
