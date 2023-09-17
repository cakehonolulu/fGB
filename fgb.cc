#include <jit/block.hh>
#include <jit/compiler.hh>
#include <fgb.hh>
#include <cpu/cpu.hh>
#include <mmu/mmu.hh>
#include <iostream>
#include <string>
#include <vector>
#include <cstddef>
#include <fstream>

int main(int argc, char *argv[])
{
	bool enable_jit = false, provided_bootrom = false, provided_rom = false;
	std::string bootrom_path, rom_path;

	if (argc < 2)
	{
		std::cout << "Usage: " << argv[0] << " <args> ...\n";
		std::cout << "|      -jit -> Enables the (Alpha) JIT compiler\n";
		return 1;
	}
	else
	{
		std::string jit_arg = "-jit";
		std::string bootrom_arg = "-bootrom";
		std::string rom_arg = "-rom";

		for (int i = 0; i < argc; i++)
		{
			if (jit_arg.compare(argv[i]) == 0)
			{
				std::cout << BOLDBLUE << "Enabling JIT mode...!" << RESET << "\n";
				enable_jit = true;
			}
			else
			if (bootrom_arg.compare(argv[i]) == 0)
			{
				if (argv[i + 1] != NULL)
				{
					provided_bootrom = true;
					bootrom_path = argv[i + 1];
					i++;
				}
				else
				{
					std::cout << BOLDRED << "No bootrom file provided...!" << RESET << "\n";
				}
			}
			else
			if (rom_arg.compare(argv[i]) == 0)
			{
				if (argv[i + 1] != NULL)
				{
					provided_rom = true;
					rom_path = argv[i + 1];
					i++;
				}
				else
				{
					std::cout << BOLDRED << "No ROM file provided...!" << RESET << "\n";
				}
			}
		}
	}

	if (!provided_bootrom)
	{
		std::cout << BOLDRED << "No bootrom provided...! Exiting..." << RESET << "\n";
		exit(1);
	}

    // Open the file in binary mode
    std::ifstream file(bootrom_path, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << bootrom_path << std::endl;
        return 1;
    }

    // Get the file size
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Create a vector to store the file contents
    std::vector<char> fileData(fileSize);

    // Read the file into the vector
    if (!file.read(fileData.data(), fileSize)) {
        std::cerr << "Failed to read file: " << bootrom_path << std::endl;
        return 1;
    }

    // Close the file
    file.close();

	Cpu cpu = Cpu();
	Mmu mmu = Mmu(&fileData);

	if (provided_rom)
	{
		// Open the file in binary mode
		std::ifstream rom_file(rom_path, std::ios::binary | std::ios::ate);

		if (!rom_file.is_open()) {
			std::cerr << "Failed to open file: " << rom_path << std::endl;
			return 1;
		}

		// Get the file size
		std::streampos rom_size = rom_file.tellg();
		rom_file.seekg(0, std::ios::beg);

		// Create a vector to store the file contents
		std::vector<char> rom_data(rom_size);

		// Read the file into the vector
		if (!rom_file.read(rom_data.data(), rom_size)) {
			std::cerr << "Failed to read file: " << rom_path << std::endl;
			return 1;
		}

		// Close the file
		rom_file.close();

		mmu.load_game(&rom_data, rom_size);
	}

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