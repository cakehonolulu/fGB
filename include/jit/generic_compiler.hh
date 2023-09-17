#pragma once

#include <cstdint>
#include <iostream>
#include <jit/generic_emitter.hh>
#include <jit/generic_block.hh>

#ifdef JIT_AMD64
#include <jit/amd64/amd64_block.hh>
#include <jit/amd64/amd64_emitter.hh>
#endif

#include <mmu/mmu.hh>
#include <cpu/cpu.hh>
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

#ifdef JIT_AMD64
    void run(Cpu *cpu, Amd64_Emitter *emitter, Mmu *mmu);
#endif

    JitBlock *jit_find_compiled_block(std::uint16_t pc);

    JitBlock *jit_find_available_block();
};
