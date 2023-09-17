#pragma once

#include <jit/generic_block.hh>
#include <capstone/capstone.h>
#include <xbyak/xbyak.h>
#include <cinttypes>
#include <cstdint>

/* Customized Block Class for AMD64 Translations */

class JitBlock : public Block, public Xbyak::CodeGenerator {

public:
    void disassemble();

    int execute();
    size_t get_size();
    const uint8_t *get_location();
};
