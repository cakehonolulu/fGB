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
    uint16_t id;

public:

    JitBlock();

    JitBlock(uint8_t id_, uint16_t pc_);

    ~JitBlock();

    bool is_dirty();

    void set_dirty(bool dirty_);

    std::uint16_t get_pc();

    void set_pc(std::uint16_t pc_);
    
    std::uint16_t get_id();

    void set_id(std::uint16_t id_);

    int execute();

    size_t get_size();

    const uint8_t *get_location();

    void disassemble();
};
