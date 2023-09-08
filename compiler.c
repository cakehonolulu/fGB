#include <compiler.h>
#include <emitter.h>
#include <instruction_tables.h>
#include <Zydis/Zydis.h>
#include <inttypes.h>
#include <sys/mman.h>
#include <unistd.h>

jit_t jit = { 0 };

jit_t jit_init(cpu_t *cpu)
{
    // Allocate the blocks
    jit.blocks = malloc(sizeof(jit_block_t) * MAX_BLOCKS);
    memset(jit.blocks, 0, sizeof(jit_block_t) * MAX_BLOCKS);

    for (uint8_t i = 0; i < MAX_BLOCKS; i++)
    {
        jit.blocks[i].data = malloc(BLOCK_SIZE * sizeof(uint8_t));
        jit.blocks[i].id = i;
        jit.blocks[i].pc = 0xAABB;
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
    printf("Searching a block matching PC = %04X...\n", cpu->pc);

    bool found = false;
    uint8_t i = 0;

    jit_block_t *block = NULL;

    while (!found && i < MAX_BLOCKS)
    {
        if (jit.blocks[i].pc == cpu->pc)
        {
            found = true;
            block = &jit.blocks[i];
        }

        i++;
    }

    if (found)
    {
        // Execute block
        printf("Block with matching PC found (Block ID: %d), executing...\n", block->id);
        printregs(cpu);
        exit(1);
    }
    else
    {
        printf("Didn't find a block with PC = %04X, building a new block...\n", cpu->pc);
    
        bool condition = false;

        uint16_t prev_pc = cpu->pc;

        jit_block_t *target_block = NULL;

        target_block = jit_process_instruction_block(cpu);

        printf("Emitting structure restoring bytecode...\n");

        restore_cpu_struct(target_block, cpu);

        target_block->data[target_block->current_len++] = 0xC3; // RET
        printf("Ended up processing a block...!\n");
        

        //printf("Trying to execute block (ID: %d)...\n", target_block->id);

        printf("\n");
        //printregs(cpu);
        //printblocks(&jit);

        printblock(&jit, target_block);

        printf("\nDumping block bytecode...\n");

        int j = 0;

        for (j; j < target_block->current_len; j++)
        {
            printf("%02X ", target_block->data[j]);
        }

        printf("\n");

        jit_execute_block(cpu, target_block);

        //cpu->pc = prev_pc;
    }
}

void restore_cpu_struct(jit_block_t *block, cpu_t *cpu)
{
    if (block->current_len + 24 > BLOCK_SIZE)
    {
        // TODO: Handle reallocation
        printf("Block size exceeded, realloc?\n");
        exit(1);
    }
    else
    {
        // mov rdi, &cpu
        EMIT(0x48);
        EMIT(0xBF);
        EMIT((((uintptr_t) cpu)) & 0x00000000000000FF);
        EMIT((((uintptr_t) cpu)  & 0x000000000000FF00) >> 8);
        EMIT((((uintptr_t) cpu)  & 0x0000000000FF0000) >> 16);
        EMIT((((uintptr_t) cpu)  & 0x00000000FF000000) >> 24);
        EMIT((((uintptr_t) cpu)  & 0x000000FF00000000) >> 32);
        EMIT((((uintptr_t) cpu)  & 0x0000FF0000000000) >> 40);
        EMIT((((uintptr_t) cpu)  & 0x00FF000000000000) >> 48);
        EMIT((((uintptr_t) cpu)  & 0xFF00000000000000) >> 56);
        
        /*
            Register Allocation (x86_64):

            Host                    Target
            AX                      AF
            BX                      BC
            CX                      HL
            High 32 bits RDX        PC 
            Low 32 bits RDX         SP
        */

        // AF
        // mov [rdi + 0], ax
        EMIT(0x66);
        EMIT(0x89);
        EMIT(0x07);

        // BC
        // mov [rdi + 2], bx
        EMIT(0x66);
        EMIT(0x89);
        EMIT(0x5F);
        EMIT(0x02);

        // HL
        // mov [rdi + 4], cx
        EMIT(0x66);
        EMIT(0x89);
        EMIT(0x4F);
        EMIT(0x04);

        // SP
        // mov [rdi + 8], edx
        EMIT(0x89);
        EMIT(0x57);
        EMIT(0x08);
    }
}

void jit_execute_block(cpu_t *cpu, jit_block_t *block)
{
    printf("Allocating memory for the exec'd buffer...\n");
    uint8_t *buffer = malloc(sizeof(uint8_t) * block->current_len);

    uint8_t j = 0;

    while (j < block->current_len)
    {
        buffer[j] = block->data[j];
        j++;
    }

    if (posix_memalign((void *) &buffer, sysconf(_SC_PAGESIZE), sizeof(uint8_t) * block->current_len) != 0) {
        printf("Failed to allocate code buffer\n");
        exit(1);
    }
    else
    {

    printf("Allocated memory is now aligned\n");
    }

    if (mprotect(buffer, sizeof(uint8_t) * block->current_len, PROT_READ | PROT_WRITE | PROT_EXEC) == -1) {
        printf("mprotect error\n");
        free(buffer);
        exit(1);
    }
    else
    {

    printf("Page permissions set correctly\n");
    }

    printf("Copying instructions...\n");
    memcpy(buffer, block->data, sizeof(uint8_t) * block->current_len);

    printf("Jumping to compiled code...\n\n\n");
    // Cast the buffer to a function pointer and execute it
    void (*jitFunction)() = (void *) buffer;
    jitFunction();
}

jit_block_t *jit_process_instruction_block(cpu_t *cpu)
{
    jit_block_t *target_block = jit_find_available_block(cpu);

    printf("Starting to process instructions (PC: %04X)\n", cpu->pc);

    uint16_t saved_pc = cpu->pc;

    bool cond = false;

    uint8_t instr = 0;

    while (!cond)
    {
        instr = cpu->bootrom_buffer[cpu->pc];
        cond = jit_translate(instr, target_block, cpu);
    }

    return target_block;
}

jit_block_t *jit_find_available_block(cpu_t *cpu)
{
    printf("Searching for an available block...\n");

    bool found = false;
    uint8_t current_block = 0;
    jit_block_t *block = NULL;

    while (!found && current_block < MAX_BLOCKS)
    {
        if (!jit.blocks[current_block].dirty)
        {
            printf("Found a non-dirty and free block!\n");
            found = true;
            block = &jit.blocks[current_block];
            emit_caller(cpu, block);
        }
        else if (jit.blocks[current_block].current_len < MAX_BLOCKS)
        {
            printf("Found a dirty block, skipping!\n");
        }

        current_block++;
    }

    return block;
}

void emit_caller(cpu_t *cpu, jit_block_t *block)
{
    printf("Emitting caller instructions in non-dirty block (Block ID: %d)...\n", block->id);

    // Set the block's identifying PC to the current one
    block->pc = cpu->pc;

    // Map target registers to host registers if block is not dirty

    // mov rdi, &cpu
    EMIT(0x48);
    EMIT(0xBF);
    EMIT((((uintptr_t) cpu)) & 0x00000000000000FF);
    EMIT((((uintptr_t) cpu)  & 0x000000000000FF00) >> 8);
    EMIT((((uintptr_t) cpu)  & 0x0000000000FF0000) >> 16);
    EMIT((((uintptr_t) cpu)  & 0x00000000FF000000) >> 24);
    EMIT((((uintptr_t) cpu)  & 0x000000FF00000000) >> 32);
    EMIT((((uintptr_t) cpu)  & 0x0000FF0000000000) >> 40);
    EMIT((((uintptr_t) cpu)  & 0x00FF000000000000) >> 48);
    EMIT((((uintptr_t) cpu)  & 0xFF00000000000000) >> 56);
    
    /*
        Register Allocation (x86_64):

        Host                    Target
        AX                      AF
        BX                      BC
        CX                      HL
        High 32 bits RDX        PC 
        Low 32 bits RDX         SP
    */

    // AF
    // mov ax, [rdi + 0]
    EMIT(0x66);
    EMIT(0x8B);
    EMIT(0x07);

    // BC
    // mov bx, [rdi + 2]
    EMIT(0x66);
    EMIT(0x8B);
    EMIT(0x5F);
    EMIT(0x02);

    // HL
    // mov cx, [rdi + 4]
    EMIT(0x66);
    EMIT(0x8B);
    EMIT(0x4F);
    EMIT(0x04);

    // SP
    // mov edx, [rdi + 8]
    EMIT(0x8B);
    EMIT(0x57);
    EMIT(0x08);

    printf("Space left for translated code: %d\n", BLOCK_SIZE - block->current_len);

    block->dirty = true;
}

bool jit_translate(uint8_t instruction, jit_block_t *block, cpu_t *cpu)
{
    bool result = false;

	switch (instruction)
    {
        case 0x20:
            result = true;
            break;

        case 0x21:
            jit_emit_21(block, cpu);
            break;

        case 0x31:
            jit_emit_31(block, cpu);
            break;

        case 0x32:
            printf("[JIT] LD (HL-), A (Dummy)\n");
            cpu->pc++;
            break;

        case 0xAF:
            jit_emit_AF(block, cpu);
            break;

        case 0xCB:
            switch (cpu->bootrom_buffer[cpu->pc + 1])
            {
                case 0x7C:
                    printf("[JIT] BIT 7, H (Dummy)\n");
                    cpu->pc++;
                    break;

                default:
                    printf("[JIT] Unhandled Extended 0xCB instruction: 0x%02X\n", cpu->bootrom_buffer[cpu->pc + 1]);
	                printregs(cpu);
                    printblocks(&jit);
	                exit(1);
                    break;
            }
            cpu->pc++;
            break;

        default:
	        printf("[JIT] Unhandled instruction: 0x%02X\n", instruction);
	        printregs(cpu);
            printblocks(&jit);
	        exit(1);
            break;
    }

    return result;
}

void printblocks(jit_t *jit)
{
    jit_block_t *current_block = NULL;
    ZyanU8 *instruction_buffer = NULL;

    printf("---------- BLOCK INFORMATION ----------\n");
    
    for (uint8_t i = 0; i < MAX_BLOCKS; i++)
    {
        if (jit->blocks[i].dirty && jit->blocks[i].current_len)
        {
            current_block = &jit->blocks[i];

            printf("Block (ID: %d, Free Bytes: %d) Dump:\n", current_block->id, BLOCK_SIZE - current_block->current_len);

            instruction_buffer = malloc(sizeof(ZyanU8) * current_block->current_len);

            uint8_t j = 0;

            while (j < current_block->current_len)
            {
                instruction_buffer[j] = current_block->data[j];
                j++;
            }
            
            /* Snippet taken from Zydis GitHub Example */
            ZyanU64 runtime_address = (ZyanU64) current_block->data;

            // Loop over the instructions in our buffer.
            ZyanUSize offset = 0;
            ZydisDisassembledInstruction instruction;
            while (ZYAN_SUCCESS(ZydisDisassembleIntel(
                /* machine_mode:    */ ZYDIS_MACHINE_MODE_LONG_64,
                /* runtime_address: */ runtime_address,
                /* buffer:          */ instruction_buffer + offset,
                /* length:          */ current_block->current_len - offset,
                /* instruction:     */ &instruction
            ))) {
                printf("%016" PRIX64 "  %s\n", runtime_address, instruction.text);
                offset += instruction.info.length;
                runtime_address += instruction.info.length;
            }

            free(instruction_buffer);
        }
    }
}

void printblock(jit_t *jit, jit_block_t *block)
{
    ZyanU8 *instruction_buffer = NULL;

    printf("---------- BLOCK INFORMATION ----------\n");

    printf("Block (ID: %d, Free Bytes: %d) Dump:\n", block->id, BLOCK_SIZE - block->current_len);

    instruction_buffer = malloc(sizeof(ZyanU8) * block->current_len);

    uint8_t j = 0;

    while (j < block->current_len)
    {
        instruction_buffer[j] = block->data[j];
        j++;
    }
            
    /* Snippet taken from Zydis GitHub Example */
    ZyanU64 runtime_address = (ZyanU64) block->data;

    // Loop over the instructions in our buffer.
    ZyanUSize offset = 0;
    ZydisDisassembledInstruction instruction;
    while (ZYAN_SUCCESS(ZydisDisassembleIntel(
        /* machine_mode:    */ ZYDIS_MACHINE_MODE_LONG_64,
        /* runtime_address: */ runtime_address,
        /* buffer:          */ instruction_buffer + offset,
        /* length:          */ block->current_len - offset,
        /* instruction:     */ &instruction
    ))) {
        printf("%016" PRIX64 "  %s\n", runtime_address, instruction.text);
        offset += instruction.info.length;
        runtime_address += instruction.info.length;
    }

    free(instruction_buffer);
}

void jit_emit_21(jit_block_t *block, cpu_t *cpu)
{
    printf("[JIT] LD HL, %04X\n", (uint16_t) (((cpu->bootrom_buffer[cpu->pc + 2] << 8) | (cpu->bootrom_buffer[cpu->pc + 1]))));

    // mov cx, u16
    EMIT(0x66);
    EMIT(0xB9);
    EMIT(cpu->bootrom_buffer[cpu->pc + 1]);
    EMIT(cpu->bootrom_buffer[cpu->pc + 2]);

	cpu->pc += 3;
}

void jit_emit_31(jit_block_t *block, cpu_t *cpu)
{
    printf("[JIT] LD SP, %04X\n", (uint16_t) (((cpu->bootrom_buffer[cpu->pc + 2] << 8) | (cpu->bootrom_buffer[cpu->pc + 1]))));

    // mov edx, u16
    EMIT(0xBA);
    EMIT(cpu->bootrom_buffer[cpu->pc + 1]);
    EMIT(cpu->bootrom_buffer[cpu->pc + 2]);
    EMIT(0x00);
    EMIT(0x00);

    cpu->pc += 3;
}

void jit_emit_AF(jit_block_t *block, cpu_t *cpu)
{
    printf("[JIT] XOR A, A\n");

    // xor ah, ah
    EMIT(0x30);
    EMIT(0xE4);

	cpu->pc++;

    /*
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
    */
}
