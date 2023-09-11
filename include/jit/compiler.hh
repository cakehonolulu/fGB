#pragma once

#include <cstdint>
#include <iostream>
#include <jit/emitter.hh>
#include <jit/block.hh>
#include <cpu.hh>
#include <vector>

#define BLOCK_COUNT_SZ_SAFE 16
#define BLOCK_COUNT         BLOCK_COUNT_SZ_SAFE

class Compiler {

private:
    // Blocks to be used by the compiler
    JitBlock* blocks;

    // Used blocks metric
    uint8_t used_blocks;

public:

    Compiler();

    ~Compiler();

    void run(Cpu *cpu, Emitter *emitter, std::vector<char> *bootrom);

    JitBlock *jit_find_compiled_block(std::uint16_t pc);

    JitBlock *jit_find_available_block();
};
