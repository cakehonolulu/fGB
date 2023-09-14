#include <jit/block.hh>
#include <jit/compiler.hh>
#include <fgb.hh>
#include <cpu.hh>
#include <mmu.hh>
#include <iostream>
#include <string>
#include <vector>
#include <cstddef>
#include <fstream>

int main(int argc, char *argv[])
{
	bool enable_jit = false;

	if (argc < 2)
	{
		std::cout << "Usage: " << argv[0] << " <args> ...\n";
		std::cout << "|      -jit -> Enables the (Alpha) JIT compiler\n";
		return 1;
	}
	else
	{
		std::string jit_arg = "-jit";

		for (int i = 0; i < argc; i++)
		{
			if (jit_arg.compare(argv[i]) == 0)
			{
				std::cout << BOLDBLUE << "Enabling JIT mode...!" << RESET << "\n";
				enable_jit = true;
			}
		}
	}

	
	// Specify the file path
    const std::string filePath = "bootrom.bin";

    // Open the file in binary mode
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return 1;
    }

    // Get the file size
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Create a vector to store the file contents
    std::vector<char> fileData(fileSize);

    // Read the file into the vector
    if (!file.read(fileData.data(), fileSize)) {
        std::cerr << "Failed to read file: " << filePath << std::endl;
        return 1;
    }

    // Close the file
    file.close();

	Cpu cpu = Cpu();
	Mmu mmu = Mmu(&fileData);

	if (enable_jit)
	{
		Compiler *jit_compiler = new Compiler();
		Emitter emitter = Emitter(&mmu);

		//exit(1);

		jit_compiler->run(&cpu, &emitter, &mmu);

		/*block.mov(Xbyak::util::rax, 42);
	    block.ret();

		int result = block.execute();
		std::cout << "Result of executed code: " << result << std::endl;
		block.disassemble();*/
	}
	else
	{
		std::cout << BOLDRED << "Interpreter mode under construction..." << RESET << "\n";
	}

	return 0;
}