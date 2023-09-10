#pragma once

#include <cpu.h>
#include <stdlib.h>
#include <string.h>

#define SAFE        64
#define MEDIUM      128
#define BIG         256
#define BLOCK_SIZE  SAFE

#define MAX_BLOCKS  SAFE

typedef struct {
    uint8_t *data;
    uint8_t current_len;
    uint16_t pc;
    bool dirty;
    uint8_t id;
} jit_block_t;

typedef struct {
    jit_block_t *blocks;
    int used_slots;
} jit_t;

jit_t jit_init(cpu_t *cpu);
void jit_fde(cpu_t *cpu);
void jit_process_block(jit_t *jit, cpu_t *cpu);
void jit_execute_block(cpu_t *cpu, jit_block_t *block);
jit_block_t *jit_process_instruction_block(cpu_t *cpu);
jit_block_t *jit_find_available_block(cpu_t *cpu);
void jit_emit_prologue(cpu_t *cpu, jit_block_t *block);
void jit_emit_epilogue(jit_block_t *block, cpu_t *cpu);
bool jit_translate(uint8_t instruction, jit_block_t *block, cpu_t *cpu);
void printblocks(jit_t *jit);
void printblock(jit_t *jit, jit_block_t *block);
void jit_emit_bytes(jit_block_t *block, int size, ...);

/* Instructions */
void jit_emit_20(jit_block_t *block, cpu_t *cpu);
void jit_emit_21(jit_block_t *block, cpu_t *cpu);
void jit_emit_31(jit_block_t *block, cpu_t *cpu);
void jit_emit_AF(jit_block_t *block, cpu_t *cpu);
