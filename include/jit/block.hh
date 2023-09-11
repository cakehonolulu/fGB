#pragma once

#include <cstdint>
#include <xbyak/xbyak.h>

#define BLOCK_SZ_SAFE   128

#define BLOCK_SIZE      BLOCK_SZ_SAFE

class JitBlock : public Xbyak::CodeGenerator {

private:
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

    int execute();

    size_t get_size();

    const uint8_t *get_location();

    void disassemble();
};
