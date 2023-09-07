#include <cpu.h>
#include <instruction_tables.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void printregs(cpu_t *cpu)
{
        printf("A:  0x%02X, F: 0x%02X, AF: 0x%04X\n", cpu->af.a, cpu->af.f, cpu->af.af);
        printf("B:  0x%02X, C: 0x%02X, BC: 0x%04X\n", cpu->bc.b, cpu->bc.c, cpu->bc.bc);
        printf("H:  0x%02X, L: 0x%02X, HL: 0x%04X\n", cpu->hl.h, cpu->hl.l, cpu->hl.hl);
        printf("PC: 0x%04X, SP: 0x%04X\n", cpu->pc, cpu->sp);
}

void fde(cpu_t *cpu)
{
	uint8_t instr = cpu->bootrom_buffer[cpu->pc];

	if (instrs[instr].function != NULL)
	{	
		((void (*)())instrs[instr].function)();
	}
	else
	{
		printf("Unhandled instruction: 0x%02X\n", instr);
		printregs(cpu);
		exit(1);
	}
}

int main(void)
{
	FILE *bootrom = fopen("bootrom.bin", "ro");

	if (bootrom == NULL)
		return 1;

	// Get file size in bytes
	fseek(bootrom, 0, SEEK_END);
	size_t bootromsz = ftell(bootrom);
	fseek(bootrom, 0, SEEK_SET);

	cpu_t cpu = { 0 };

	// Allocate a buffer for the program
	cpu.bootrom_buffer = (unsigned char*) malloc(sizeof(unsigned char) * bootromsz);

	fread(cpu.bootrom_buffer, sizeof(unsigned char), bootromsz, bootrom);

	printf("Running\n");

	while (1)
	{
		fde(&cpu);
	}

	return 0;
}
