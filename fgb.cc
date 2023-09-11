#include <iostream>
#include <string>
#include <jit/block.hh>
#include <jit/compiler.hh>
#include <fgb.hh>

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

	if (enable_jit)
	{
		//Compiler *jit_compiler = new Compiler();
		JitBlock block = JitBlock();

		
		block.mov(Xbyak::util::rax, 42);
	    block.ret();

		int result = block.execute();
		std::cout << "Result of executed code: " << result << std::endl;
		block.disassemble();
	}
	else
	{
		std::cout << BOLDRED << "Interpreter mode under construction..." << RESET << "\n";
	}

	return 0;
}