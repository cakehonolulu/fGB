#pragma once

#include <cstdint>

#define BLOCK_SZ_SAFE   128

#define BLOCK_SIZE      BLOCK_SZ_SAFE

class JitBlock {

private:
    // Pointer to the memory block containing the translated instructions
    uintptr_t *data;

    // The current block length
    uint8_t current_len;

    // The Program Counter value that identifies the block
    uint16_t pc;

    // Block use state
    bool dirty;

    // ID of the block
    uint8_t id;

public:

    JitBlock ();

    JitBlock (uint8_t id_, uint16_t pc_);

    ~JitBlock();
};
