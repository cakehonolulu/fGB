#include <compiler.h>
#include <instruction_tables.h>

jit_t jit = { 0 };

jit_t jit_init(cpu_t *cpu)
{
    // Allocate the blocks
    jit.blocks = malloc(sizeof(jit_block_t) * MAX_BLOCKS);
    memset(jit.blocks, 0, sizeof(jit_block_t) * MAX_BLOCKS);

    for (uint8_t i = 0; i < MAX_BLOCKS; i++)
    {
        jit.blocks[i].data = malloc(BLOCK_SIZE * sizeof(size_t));
    }
}

void jit_fde(cpu_t *cpu)
{
    /*
        TODO: Lookup the block on the block array, maybe it exists
        already (Need a way to map the start of the target block
        to the host one to properly compare them).
    */
   jit_process_block(cpu);
}

void jit_process_block(cpu_t *cpu)
{
    uint8_t instr = cpu->bootrom_buffer[cpu->pc];

    jit_process_instruction(instr, cpu);
}

void jit_process_instruction(uint8_t instruction, cpu_t *cpu)
{
    jit_block_t *target_block = jit_find_available_block(cpu);

    jit_translate(instruction, target_block, cpu);
}

jit_block_t *jit_find_available_block(cpu_t *cpu)
{
    bool found = false;
    uint8_t current_block = 0;
    jit_block_t *block = NULL;

    while (!found || current_block < MAX_BLOCKS)
    {
        if (!jit.blocks[current_block].dirty)
        {
            found = true;
            block = &jit.blocks[current_block];
            emit_caller(cpu, block);
        }
        else if (jit.blocks[current_block].current_len < MAX_BLOCKS)
        {
            found = true;
            block = &jit.blocks[current_block];
        }

        current_block++;
    }

    return block;
}

void emit_caller(cpu_t *cpu, jit_block_t *block)
{
    printf("Emitting caller instructions in non-dirty block...\n");

    // Map target registers to host registers if block is not dirty

    // MOV EAX, pointer
    block->data[block->current_len++] = 0xB9;                                   // MOV EAX
    block->data[block->current_len++] = (((uintptr_t) cpu)) & 0x000000FF;       // POINTER LSB
    block->data[block->current_len++] = (((uintptr_t) cpu) & 0x0000FF00) >> 8;
    block->data[block->current_len++] = (((uintptr_t) cpu) & 0x00FF0000) >> 16;
    block->data[block->current_len++] = (((uintptr_t) cpu) & 0xFF000000) >> 24; // POINTER MSB

    block->dirty = true;    
}

void jit_translate(uint8_t instruction, jit_block_t *block, cpu_t *cpu)
{
	switch (instruction)
    {
        case 0x31:
            //jit_emit_31(block, cpu);
            break;

        default:
	        printf("[JIT] Unhandled instruction: 0x%02X\n", instruction);
	        printregs(cpu);
	        exit(1);
            break;
    }
}

void jit_emit_31(jit_block_t *block, cpu_t *cpu)
{
    printf("[JIT] LD SP, %04X\n", (uint16_t) (((cpu->bootrom_buffer[cpu->pc + 2] << 8) | (cpu->bootrom_buffer[cpu->pc + 1]))));
	/*
        cpu->sp = (uint16_t) (((cpu->bootrom_buffer[cpu->pc + 2] << 8) | (cpu->bootrom_buffer[cpu->pc + 1])));
	    cpu->pc += 3;
    */
}
