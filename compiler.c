#include <compiler.h>
#include <emitter.h>
#include <instruction_tables.h>
#include <termui/colors.h>
#include <Zydis/Zydis.h>
#include <inttypes.h>
#include <sys/mman.h>
#include <stdarg.h>
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

	return jit;
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
	printf(BOLD "[" BLUE "*" NORMAL BOLD "] Searching for a compiled block where PC = $%04X..." NORMAL "\n", cpu->pc);

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
		printf(BOLD "[" GREEN "✓" NORMAL BOLD "] Block with matching PC found (Block ID: %d), executing...!" NORMAL "\n", block->id);

		//jit_execute_block(cpu, block);

		printregs(cpu);
		exit(1);
	}
	else
	{
		printf(BOLD "[" RED "-" NORMAL BOLD "] No block found for PC = $%04X, building a new one..." NORMAL "\n", cpu->pc);
	
		bool condition = false;

		uint16_t prev_pc = cpu->pc;

		jit_block_t *target_block = NULL;

		target_block = jit_process_instruction_block(cpu);

#ifdef DEBUG
		printf("Emitting structure restoring bytecode...\n");
#endif

		jit_emit_epilogue(target_block, cpu);

		printf(BOLD "[" GREEN "✓" NORMAL BOLD "] Ended up processing a block...!" NORMAL "\n");
		

		//printf("Trying to execute block (ID: %d)...\n", target_block->id);

		//printregs(cpu);
		//printblocks(&jit);

#ifdef DEBUG
		printf("\nDumping block bytecode...\n");

		int j = 0;

		for (j; j < target_block->current_len; j++)
		{
			printf("%02X ", target_block->data[j]);
		}

		printf("\n");
#endif

		printf(BOLD "[" BLUE "*" NORMAL BOLD "] Executing block (Block ID: %d)..." NORMAL "\n", target_block->id);
		printf("\n");
		
		printblock(&jit, target_block);

		jit_execute_block(cpu, target_block);

		//cpu->pc = prev_pc;
	}
}

void dump_block_bytecode(jit_block_t *block)
{
	printf("\nDumping block bytecode...\n");

	int j = 0;

	for (j; j < block->current_len; j++)
	{
		printf("%02X ", block->data[j]);
	}

	printf("\n");
}

void jit_emit_epilogue(jit_block_t *block, cpu_t *cpu)
{
	if (block->current_len + 25 > BLOCK_SIZE)
	{
		// TODO: Handle reallocation
		printf("Block size exceeded, realloc?\n");
		exit(1);
	}
	else
	{
		// mov rdi, &cpu
		movq_reg64_imm64(rdi, (uintptr_t) cpu);
		
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
		EMIT_BYTE(0x66);
		EMIT_BYTE(0x89);
		EMIT_BYTE(0x07);

		// BC
		// mov [rdi + 2], bx
		EMIT_BYTE(0x66);
		EMIT_BYTE(0x89);
		EMIT_BYTE(0x5F);
		EMIT_BYTE(0x02);

		// HL
		// mov [rdi + 4], cx
		EMIT_BYTE(0x66);
		EMIT_BYTE(0x89);
		EMIT_BYTE(0x4F);
		EMIT_BYTE(0x04);

		// SP
		// mov [rdi + 8], edx
		EMIT_BYTE(0x89);
		EMIT_BYTE(0x57);
		EMIT_BYTE(0x08);

		// RET
		EMIT_BYTE(0xC3);
	}
}

void jit_execute_block(cpu_t *cpu, jit_block_t *block)
{
#ifdef DEBUG
	printf("Allocating memory for the exec'd buffer...\n");
#endif
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
#ifdef DEBUG
		printf("Allocated memory is now aligned\n");
#endif
	}

	if (mprotect(buffer, sizeof(uint8_t) * block->current_len, PROT_READ | PROT_WRITE | PROT_EXEC) == -1) {
		printf("mprotect error\n");
		free(buffer);
		exit(1);
	}
	else
	{
#ifdef DEBUG
		printf("Page permissions set correctly\n");
#endif
	}

#ifdef DEBUG
		printf("Copying instructions...\n");
#endif
	
	memcpy(buffer, block->data, sizeof(uint8_t) * block->current_len);

#ifdef DEBUG
	printf("Jumping to compiled code...\n\n\n");
#endif

	// Cast the buffer to a function pointer and execute it
	void (*jitFunction)() = (void *) buffer;
	jitFunction();

	printf(BOLD "[" GREEN "✓" NORMAL BOLD "] Block (Block ID: %d) executed successfully!" NORMAL "\n", block->id);
}

jit_block_t *jit_process_instruction_block(cpu_t *cpu)
{
	jit_block_t *target_block = jit_find_available_block(cpu);

#ifdef DEBUG
	printf("Starting to process instructions where PC: %04X\n", cpu->pc);
#endif

	uint16_t saved_pc = cpu->pc;

	bool is_branch_instr = false;

	uint8_t instr = 0;

	while (!is_branch_instr)
	{
		instr = cpu->bootrom_buffer[cpu->pc];
		is_branch_instr = jit_translate(instr, target_block, cpu);

		if (is_branch_instr)
		{
			printf(BOLD BRIGHT_YELLOW BLACK "[" RED "!" BLACK "] " RED "Found a branching instruction!" NORMAL "\n");
		}
	}

	return target_block;
}

jit_block_t *jit_find_available_block(cpu_t *cpu)
{
	bool found = false;
	uint8_t current_block = 0;
	jit_block_t *block = NULL;

	while (!found && current_block < MAX_BLOCKS)
	{
		if (!jit.blocks[current_block].dirty)
		{
#ifdef DEBUG
			printf("Found an available, non-dirty block emplacement!\n");
#endif
			found = true;
			block = &jit.blocks[current_block];
			jit_emit_prologue(cpu, block);
		}
		else if (jit.blocks[current_block].current_len < MAX_BLOCKS)
		{
#ifdef DEBUG
			printf("Found a dirty block, skipping!\n");
#endif
		}

		current_block++;
	}

	return block;
}

void jit_emit_prologue(cpu_t *cpu, jit_block_t *block)
{
	printf(BOLD "[" BLUE "*" NORMAL BOLD "] Emitting instructions for block (Block ID: %d)..." NORMAL "\n", block->id);

	// Set the block's identifying PC to the current one
	block->pc = cpu->pc;

	// Map target registers to host registers if block is not dirty

	// mov rdi, &cpu
	movq_reg64_imm64(rdi, (uintptr_t) cpu);

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
	EMIT_BYTE(0x66);
	EMIT_BYTE(0x8B);
	EMIT_BYTE(0x07);

	// BC
	// mov bx, [rdi + 2]
	movb_reg16_indirect_reg64_offset(bx, rdi, 2);

	// HL
	// mov cx, [rdi + 4]
	movb_reg16_indirect_reg64_offset(cx, rdi, 4);
	
	// SP
	// mov edx, [rdi + 8]
	EMIT_BYTE(0x8B);
	EMIT_BYTE(0x57);
	EMIT_BYTE(0x08);

#ifdef DEBUG
	printf("Space left for translated code: %d\n", BLOCK_SIZE - block->current_len);
#endif

	block->dirty = true;
}

bool jit_translate(uint8_t instruction, jit_block_t *block, cpu_t *cpu)
{
	bool is_branch_instr = false;

	switch (instruction)
	{
		case 0x20:
			jit_emit_20(block, cpu);
			is_branch_instr = true;
			break;

		case 0x21:
			jit_emit_21(block, cpu);
			break;

		case 0x31:
			jit_emit_31(block, cpu);
			break;

		case 0x32:
			printf("[JIT] " BOLD" $%04X " NORMAL" LD (HL-), A (Dummy)\n", cpu->pc);
			cpu->pc++;
			break;

		case 0xAF:
			jit_emit_AF(block, cpu);
			break;

		case 0xCB:
			switch (cpu->bootrom_buffer[cpu->pc + 1])
			{
				case 0x7C:
					printf("[JIT] " BOLD" $%04X " NORMAL" BIT 7, H (Dummy)\n", cpu->pc);
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

	return is_branch_instr;
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

	printf("Block (ID: %d, Free Bytes: %d):\n", block->id, BLOCK_SIZE - block->current_len);

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

	printf("\n");

	free(instruction_buffer);
}

void jit_emit_20(jit_block_t *block, cpu_t *cpu)
{
	printf("[JIT] " BOLD" $%04X " NORMAL" JR NZ, %d (Dummy)\n", cpu->pc, ((int8_t) (cpu->bootrom_buffer[cpu->pc + 1])));

	/*
		TODO: Check (And maybe chain?) blocks if the jump is relative (For loops or while loops)
		to avoid going through the dispatcher again and again
	*/

	int8_t displacement = (int8_t) (cpu->bootrom_buffer[cpu->pc + 1]);

	cpu->pc += 2;
	cpu->pc += displacement;

		/*if (FLAG_CHECK(ZERO))
	{
		PC += 2;
	}
	else
	{
		// Set the PC Offset at the end of the JR NZ, s8
		PC += 2;

		// Add m_operand as an int8_t (Can go forward or backward)
		PC += (int8_t) m_s8;	
	}*/
}

void jit_emit_21(jit_block_t *block, cpu_t *cpu)
{
	printf("[JIT] " BOLD" $%04X " NORMAL" LD HL, %04X\n", cpu->pc, (uint16_t) (((cpu->bootrom_buffer[cpu->pc + 2] << 8) | (cpu->bootrom_buffer[cpu->pc + 1]))));

	// mov cx, u16
	EMIT_BYTE(0x66);
	EMIT_BYTE(0xB9);
	EMIT_BYTE(cpu->bootrom_buffer[cpu->pc + 1]);
	EMIT_BYTE(cpu->bootrom_buffer[cpu->pc + 2]);

	cpu->pc += 3;
}

void jit_emit_31(jit_block_t *block, cpu_t *cpu)
{
	printf("[JIT] " BOLD" $%04X " NORMAL" LD SP, %04X\n", cpu->pc, (uint16_t) (((cpu->bootrom_buffer[cpu->pc + 2] << 8) | (cpu->bootrom_buffer[cpu->pc + 1]))));

	// mov edx, u16
	EMIT_BYTE(0xBA);
	EMIT_BYTE(cpu->bootrom_buffer[cpu->pc + 1]);
	EMIT_BYTE(cpu->bootrom_buffer[cpu->pc + 2]);
	EMIT_BYTE(0x00);
	EMIT_BYTE(0x00);

	cpu->pc += 3;
}

void jit_emit_AF(jit_block_t *block, cpu_t *cpu)
{
	printf("[JIT] " BOLD" $%04X " NORMAL" XOR A, A\n", cpu->pc);

	// xor ah, ah
	EMIT_BYTE(0x30);
	EMIT_BYTE(0xE4);

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
