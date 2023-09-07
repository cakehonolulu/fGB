#include <extended_instructions.h>

void exinstr7C(cpu_t *cpu)
{
    printf("BIT 7, H (Dummy)\n");
    cpu->pc += 2;
}
