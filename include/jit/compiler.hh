#pragma once

#include <cstdint>
#include "block.hh"

#define BLOCK_COUNT_SZ_SAFE 16
#define BLOCK_COUNT         BLOCK_COUNT_SZ_SAFE

class Compiler {

private:
    // Blocks to be used by the compiler
    JitBlock* blocks;

    // Used blocks metric
    uint8_t used_blocks;

public:

    Compiler ();

    ~Compiler();
};
