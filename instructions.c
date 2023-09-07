#include <instructions.h>
#include <extended_instructions.h>
#include <instruction_tables.h>

void instr21(cpu_t *cpu)
{
    printf("LD HL, %04X\n", (uint16_t) (((cpu->bootrom_buffer[cpu->pc + 2] << 8) | (cpu->bootrom_buffer[cpu->pc + 1]))));
	cpu->hl.hl = (uint16_t) (((cpu->bootrom_buffer[cpu->pc + 2] << 8) | (cpu->bootrom_buffer[cpu->pc + 1])));
	cpu->pc += 3;
}

void instr31(cpu_t *cpu)
{
    printf("LD SP, %04X\n", (uint16_t) (((cpu->bootrom_buffer[cpu->pc + 2] << 8) | (cpu->bootrom_buffer[cpu->pc + 1]))));
	cpu->sp = (uint16_t) (((cpu->bootrom_buffer[cpu->pc + 2] << 8) | (cpu->bootrom_buffer[cpu->pc + 1])));
	cpu->pc += 3;
}

void instr32(cpu_t *cpu)
{
    printf("LD (HL-), A (Dummy)\n");
	cpu->pc++;
}

void instrAF(cpu_t *cpu)
{
    printf("XOR A, A\n");
	cpu->af.a = cpu->af.a ^ cpu->af.a;
	cpu->pc++;

	if (cpu->af.a)
	{
		cpu->zero = false;
	}
	else
	{
		cpu->zero = true;
	}

	cpu->negative = false;
	cpu->halfcarry = false;
	cpu->carry = false;
}

void instrCB(cpu_t *cpu)
{
	if (extended_instrs[cpu->bootrom_buffer[cpu->pc + 1]].function != NULL)
	{	
		((void (*)())extended_instrs[cpu->bootrom_buffer[cpu->pc + 1]].function)();
	}
	else
	{
		printf("Unhandled Extended 0xCB instruction: 0x%02X\n", cpu->bootrom_buffer[cpu->pc + 1]);
		printregs(cpu);
		exit(1);
	}
}
