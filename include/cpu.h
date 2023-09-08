#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct {
        union {
                struct {
                        uint8_t c;
                        uint8_t b;
                };
                uint16_t bc;
        };
} regbc;

typedef struct {
        union {
                struct {
                        uint8_t f;
                        uint8_t a;
                };
                uint16_t af;
        };
} regaf;

typedef struct {
        union {
                struct {
                        uint8_t l;
                        uint8_t h;
                };
                uint16_t hl;
        };
} reghl;

typedef struct {
        regaf af;
        regbc bc;
        reghl hl;
        uint16_t pc;
        uint16_t sp;
        bool zero;
        bool negative;
        bool halfcarry;
        bool carry;

        unsigned char *bootrom_buffer;
} cpu_t;

void printregs(cpu_t *cpu);
