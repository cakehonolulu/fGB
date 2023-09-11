#include <jit/compiler.hh>
#include <jit/emitter.hh>
#include <fgb.hh>
#include <vector>

#if __has_include(<format>)
    #include <format>
    using std::format;
#else
    #include <fmt/format.h>
    using fmt::format;
#endif

Compiler :: Compiler()
{
	blocks = new JitBlock[BLOCK_COUNT];
    used_blocks = 0;
}

Compiler :: ~Compiler() {
	delete blocks;
}

void Compiler :: run(Cpu *cpu, Emitter *emitter, std::vector<char> *bootrom) {
    
    while (1) {
        if (jit_find_compiled_block(cpu->get_pc()) != NULL)
        {
            // Execute that block
            cpu->print_regs();
            exit(1);
        }
        else
        {
            std::cout << "[" << BOLDYELLOW << "!" << RESET << "] No compiled block found! Compiling a new one...\n";

            JitBlock *new_block = jit_find_available_block();

            if (new_block != NULL)
            {
                emitter->jit_compile_block(new_block, cpu, bootrom);

                std::cout << "[" << BOLDBLUE << "*" << RESET << "] Executing the block...\n";

                new_block->execute();

                std::cout << "[" << BOLDGREEN << "✓" << RESET << "] Executed successfully...! Continuing...\n";
            }
            else
            {
                std::cout << "[" << BOLDRED << "X" << RESET << "] Ran out of available blocks! Exiting...\n";
            }
        }
    }
}

JitBlock * Compiler :: jit_find_compiled_block(std::uint16_t pc) {
    JitBlock *block = NULL;

    int i = 0;
    bool found = false;

    std::cout << "[" << BOLDBLUE << "*" << RESET << "] Searching for a compiled block where PC = " << format("{:#06x}\n", pc);

    while (i < BLOCK_COUNT && !found)
    {
        if (blocks[i].get_pc() == pc)
        {
            block = &blocks[i];
            found = true;

            std::cout << "[" << BOLDGREEN << "✓" << RESET << "] Found a compiled block!\n";
        }

        i++;
    }

    return block;
}

JitBlock * Compiler :: jit_find_available_block() {
    JitBlock *block = NULL;

    int i = 0;
    bool found = false;

    std::cout << "[" << BOLDBLUE << "*" << RESET << "] Searching for an available block...\n";

    while (i < BLOCK_COUNT && !found)
    {
        if (blocks[i].is_dirty() == false)
        {
            block = &blocks[i];
            found = true;

            std::cout << "[" << BOLDGREEN << "✓" << RESET << "] Found a free block!\n";
        }

        i++;
    }

    return block;
}
