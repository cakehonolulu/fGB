#include <cpu.h>
#include <instruction_tables.h>
#include <termui/colors.h>
#include <compiler.h>
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

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
        printf("Usage: %s <args> ...\n", argv[0]);
        return 1;
    }

	bool interpreter = false, jit = false;

	for (int i = 1; i < argc; i++)
	{
        if (strcmp(argv[i], "-jit") == 0)
		{
			if (interpreter || jit)
			{
				printf("CPU Emulation Strategy already selected, ignoring argument...");
			}
			else
			{
            	printf(BOLD MAGENTA "Running in JIT mode...!\n" NORMAL);
				jit = true;
			}
        }
		else if (strcmp(argv[i], "-interpreter") == 0)
		{
			if (interpreter || jit)
			{
				printf("CPU Emulation Strategy already selected, ignoring argument...");
			}
			else
			{
            	printf("Running in Interpreter mode...!\n");
				interpreter = true;
			}
        }
    }

	FILE *bootrom = fopen("bootrom.bin", "ro");

	if (bootrom == NULL)
		return 1;

	// Get file size in bytes
	fseek(bootrom, 0, SEEK_END);
	size_t bootromsz = ftell(bootrom);
	fseek(bootrom, 0, SEEK_SET);

	cpu_t cpu = { 0 };

#ifdef DEBUG
	printf("Offset of AF in CPU is %zu\n", offsetof(cpu_t, af.af));
	printf("Offset of BC in CPU is %zu\n", offsetof(cpu_t, bc.bc));
	printf("Offset of HL in CPU is %zu\n", offsetof(cpu_t, hl.hl));
	printf("Offset of PC in CPU is %zu\n", offsetof(cpu_t, pc));
	printf("Offset of SP in CPU is %zu\n", offsetof(cpu_t, sp));
	printf("Offset of Zero Flag in CPU is %zu\n", offsetof(cpu_t, zero));
	printf("Offset of Negative Flag in CPU is %zu\n", offsetof(cpu_t, negative));
	printf("Offset of Half Carry Flag in CPU is %zu\n", offsetof(cpu_t, halfcarry));
	printf("Offset of Carry Flag in CPU is %zu\n", offsetof(cpu_t, carry));
#endif

	// Allocate a buffer for the program
	cpu.bootrom_buffer = (unsigned char*) malloc(sizeof(unsigned char) * bootromsz);

	fread(cpu.bootrom_buffer, sizeof(unsigned char), bootromsz, bootrom);

	if (interpreter && !jit)
	{
		while (1)
		{
			fde(&cpu);
		}
	}
	else if (!interpreter && jit)
	{
		jit_t jit = jit_init(&cpu);

		while (1)
		{
			jit_process_block(&jit, &cpu);
		}
	}
	else
	{
		printf("Wrong options, exiting...\n");
	}

	return 0;
}
