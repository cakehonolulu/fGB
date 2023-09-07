#pragma once

#include <cpu.h>
#include <stdlib.h>
#include <string.h>

#define SAFE        16
#define MEDIUM      32
#define BIG         64
#define BLOCK_SIZE  SAFE

#define MAX_BLOCKS  SAFE

typedef struct {
    uintptr_t *data;
    uint8_t current_len;
    bool dirty;
} jit_block_t;

typedef struct {
    jit_block_t *blocks;
    int used_slots;
} jit_t;

jit_t jit_init(cpu_t *cpu);
void jit_fde(cpu_t *cpu);
void jit_process_block(cpu_t *cpu);
void jit_process_instruction(uint8_t instruction, cpu_t *cpu);
jit_block_t *jit_find_available_block(cpu_t *cpu);
void jit_translate(uint8_t instruction, jit_block_t *block, cpu_t *cpu);
