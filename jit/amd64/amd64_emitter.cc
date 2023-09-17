#include <jit/amd64/amd64_emitter.hh>

using namespace Xbyak;
using namespace Xbyak::util;

#if __has_include(<format>)
    #include <format>
    using std::format;
#else
    #include <fmt/format.h>
    using fmt::format;
#endif

void Amd64_Emitter :: jit_compile_block(JitBlock *block, Cpu *cpu, Mmu *mmu) {

    std::cout << "[" << BOLDBLUE << "*" << RESET << "] Emitting instructions...\n";

	block->set_dirty(true);
	block->set_pc(cpu->get_pc());

	bool branch_found = false;

    jit_emit_prologue(block, cpu, mmu);

	std::uint8_t instruction;

	while (!branch_found)
	{
		instruction = mmu->read_byte(cpu->get_pc());

    	branch_found = jit_process_opcode(instruction, block, cpu, mmu);
	}

	jit_emit_epilogue(block, cpu, mmu);

    //block->disassemble();

	std::cout << "[" << BOLDRED << "!" << RESET << "] Branch found!\n";

	//cpu->print_regs();
}


bool Amd64_Emitter :: jit_process_opcode(std::uint8_t opcode, JitBlock *block, Cpu *cpu, Mmu *mmu) {
    bool branch_found = false;

	Instruction_ func = instructions[opcode].func;

    if (func != NULL)
    {
        branch_found = (this->*instructions[opcode].func)(block, cpu, mmu);
    }
	else
	{
        std::cout << BOLDRED << "execute: Instruction " << format("{:#04x}", opcode) << " unimplemented!" << RESET << std::endl;
		
		block->disassemble();

		cpu->print_regs();

		exit(1);
    }

    return branch_found;
}


bool Amd64_Emitter :: jit_process_extended_opcode(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	bool branch_found = false;

	cpu->set_pc(cpu->get_pc() + 1);

	block->inc(r9w);

	Instruction_ func = extended_instructions[mmu->read_byte(cpu->get_pc())].func;
	
    if (func != NULL)
    {	
        branch_found = (this->*extended_instructions[mmu->read_byte(cpu->get_pc())].func)(block, cpu, mmu);
    }
	else
	{
        std::cout << BOLDRED << "execute: Extended Instruction " << format("{:#04x}", mmu->read_byte(cpu->get_pc())) << " unimplemented!" << RESET << std::endl;
		
		block->disassemble();

		cpu->print_regs();

		exit(1);
    }

    return branch_found;
}


void Amd64_Emitter :: jit_emit_prologue(JitBlock *block, Cpu *cpu, Mmu *mmu) {
    /*
		Register Allocation (x86_64):

		Host                    Target
		AX                      AF
		BX                      BC
		CX                      HL
		DX						DE
		R8.W					SP
		R9.W					PC
	*/

	// Map target registers to host registers if block is not dirty

	// mov rdi, &cpu
	block->mov(rdi, (size_t)cpu);

	// AF
	block->mov(AF, ptr [rdi]);

	// BC
	block->mov(BC, ptr [rdi + 2]);

	// DE
	block->mov(DE, ptr [rdi + 4]);

	// HL
	block->mov(HL, ptr [rdi + 6]);

	// PC
	block->mov(PC, ptr [rdi + 8]);

	// SP
	block->mov(SP, ptr [rdi + 10]);
}

void Amd64_Emitter :: jit_emit_epilogue(JitBlock *block, Cpu *cpu, Mmu *mmu) {
    /*
		Register Allocation (x86_64):

		Host                    Target
		AX                      AF
		BX                      BC
		CX                      HL
		DX						DE
		R8.W					SP
		R9.W					PC
	*/

	// Map target registers to host registers if block is not dirty

	// mov rdi, &cpu
	block->mov(rdi, (size_t)cpu);

	// AF
	block->mov(ptr [rdi], AF);

	// BC
	block->mov(ptr [rdi + 2], BC);

	// DE
	block->mov(ptr [rdi + 4], DE);

	// HL
	block->mov(ptr [rdi + 6], HL);

	// PC
	block->mov(ptr [rdi + 8], PC);

	// SP
	block->mov(ptr [rdi + 10], SP);

	// RET
	block->ret();
}

void Amd64_Emitter :: jit_restore_frame(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	block->mov(rdi, (size_t)cpu);
	block->mov(AF, ptr [rdi]);
	block->mov(BC, ptr [rdi + 2]);
	block->mov(DE, ptr [rdi + 4]);
	block->mov(HL, ptr [rdi + 6]);
	block->mov(PC, ptr [rdi + 8]);
	block->mov(SP, ptr [rdi + 10]);
}

void Amd64_Emitter :: jit_save_frame(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	block->mov(rdi, (size_t)cpu);
	block->mov(ptr [rdi], AF);
	block->mov(ptr [rdi + 2], BC);
	block->mov(ptr [rdi + 4], DE);
	block->mov(ptr [rdi + 6], HL);
	block->mov(ptr [rdi + 8], PC);
	block->mov(ptr [rdi + 10], SP);
}

void Amd64_Emitter :: handle_movz_arg(JitBlock* block, const Xbyak::Reg8& arg, std::uint8_t pos)
{
	// BYte to word
	switch (pos) {
		case 1:
    		block->movzx(arg1w, arg);
			break;
		
		case 2:
    		block->movzx(arg2w, arg);
			break;
		
		case 3:
    		block->movzx(arg3w, arg);
			break;
	}
}

void Amd64_Emitter :: handle_movz_arg(JitBlock* block, const Xbyak::Reg16& arg, std::uint8_t pos)
{
	// Word to dword
	switch (pos) {
		case 1:
    		block->movzx(arg1d, arg);
			break;
		
		case 2:
    		block->movzx(arg2d, arg);
			break;
		
		case 3:
    		block->movzx(arg3d, arg);
			break;
	}
}

void Amd64_Emitter :: handle_movz_arg(JitBlock* block, const Xbyak::Reg32& arg, std::uint8_t pos)
{
	// Dword to qword
	switch (pos) {
		case 1:
    		block->movzx(arg1q, arg);
			break;
		
		case 2:
    		block->movzx(arg2q, arg);
			break;
		
		case 3:
    		block->movzx(arg3q, arg);
			break;
	}
}

template <typename FuncType, typename Arg1Type>
void Amd64_Emitter :: call_func_one_arg(JitBlock* block, Mmu* mmu, FuncType func, const Arg1Type& arg1)
{
#if !defined (NO_64BIT)
    block->mov(arg1q, arg1);
#else
	block->mov(arg1d, arg1);
#endif

	handle_movz_arg(block, arg1, 1);

    block->mov(rax, reinterpret_cast<uintptr_t>(func));
	
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
	block->sub(rsp, 40);
#endif

#if !defined (NO_64BIT)
    block->mov(rax, reinterpret_cast<uintptr_t>(func));
#else
	block->mov(eax, reinterpret_cast<uintptr_t>(func));
#endif

#if !defined (NO_64BIT)
    block->call(rax);
#else
	block->call(eax);
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
	block->add(rsp, 40);
#endif
}

template <typename FuncType, typename Arg1Type, typename Arg2Type>
void Amd64_Emitter :: call_func_two_arg(JitBlock* block, Mmu* mmu, FuncType func, const Arg1Type& arg1, const Arg2Type& arg2)
{
#if !defined (NO_64BIT)
    block->mov(arg1q, arg1);
#else
	block->mov(arg1d, arg1);
#endif

	handle_movz_arg(block, arg2, 2);

#if !defined (NO_64BIT)
    block->mov(rax, reinterpret_cast<uintptr_t>(func));
#else
	block->mov(eax, reinterpret_cast<uintptr_t>(func));
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
	block->sub(rsp, 40);
#endif

#if !defined (NO_64BIT)
    block->call(rax);
#else
	block->call(eax);
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
	block->add(rsp, 40);
#endif
}

template <typename FuncType, typename Arg1Type, typename Arg2Type, typename Arg3Type>
void Amd64_Emitter :: call_func_three_arg(JitBlock* block, Mmu* mmu, FuncType func, const Arg1Type& arg1, const Arg2Type& arg2, const Arg3Type& arg3)
{
#if !defined (NO_64BIT)
    block->mov(arg1q, arg1);
#else
	block->mov(arg1d, arg1);
#endif

	handle_movz_arg(block, arg2, 2);
	handle_movz_arg(block, arg3, 3);

#if !defined (NO_64BIT)
    block->mov(rax, reinterpret_cast<uintptr_t>(func));
#else
	block->mov(eax, reinterpret_cast<uintptr_t>(func));
#endif


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
	block->sub(rsp, 40);
#endif

#if !defined (NO_64BIT)
    block->call(rax);
#else
	block->call(eax);
#endif
    
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
	block->add(rsp, 40);
#endif
}

bool Amd64_Emitter :: instr_06(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	std::uint8_t u8 = (mmu->read_byte(cpu->get_pc() + 1));

	std::cout << BOLDBLUE << "[JIT] LD B, $" << format("{:02X}", u8) << RESET << "\n";

	block->mov(B, u8);

	block->add(PC, 2);

	cpu->set_pc(cpu->get_pc() + 2);

	return false;
}

bool Amd64_Emitter :: instr_0c(JitBlock *block, Cpu *cpu, Mmu *mmu) {

	std::cout << BOLDBLUE << "[JIT] INC C" << RESET << "\n";

	block->inc(C);

	block->cmp(C, 0);

	block->je("@0c_set_zero_flag");

	// Unset Z bit
	block->and_(F, 0x7F);

	block->jmp("@0c_check_half_carry");

	block->L("@0c_set_zero_flag");

	// Set Z bit
	block->or_(F, 0x80);

	block->L("@0c_check_half_carry");

	// Test if bit 9 is set on BC
	block->test(BC, 0x0200);

	block->jnz("@0c_hc_bit_is_set");   // Jump if the 9th bit is set
	
	block->and_(F, 0xDF);

	block->jmp("@0c_finish");

	block->L("@0c_hc_bit_is_set");

	block->or_(F, 0x20);

	block->L("@0c_finish");

	block->inc(PC);

	cpu->set_pc(cpu->get_pc() + 1);

	return false;
}

bool Amd64_Emitter :: instr_0e(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	std::int8_t d8 = (mmu->read_byte(cpu->get_pc() + 1));

	std::cout << BOLDBLUE << "[JIT] LD C, $" << format("{:02X}", d8) << RESET << "\n";

	block->mov(C, d8);

	block->add(PC, 2);

	cpu->set_pc(cpu->get_pc() + 2);

	return false;
}

bool Amd64_Emitter :: instr_11(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	std::cout << BOLDBLUE << "[JIT] LD DE, $" << format("{:04X}",
		(std::uint16_t) (((0xFF & mmu->read_byte(cpu->get_pc() + 2)) << 8) | (0xFF & mmu->read_byte(cpu->get_pc() + 1)))) << RESET << std::endl;
	
	block->mov(DE, (std::uint16_t) (((0xFF & mmu->read_byte(cpu->get_pc() + 2)) << 8) | (0xFF & mmu->read_byte(cpu->get_pc() + 1))));

	cpu->set_pc(cpu->get_pc() + 3);

	block->add(PC, 3);
	return false;
}

bool Amd64_Emitter :: instr_1a(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	std::cout << BOLDBLUE << "[JIT] LD A, (DE)" << std::endl;
	
	jit_save_frame(block, cpu, mmu);

	call_func_two_arg(block, mmu, &Mmu::read_byte_wrapper, reinterpret_cast<uintptr_t>(mmu), dx);

	block->movzx(BC, F);
	block->mov(rdi, (size_t)cpu);
	block->mov(F, ptr [rdi]);
	block->mov(A, C);
	block->mov(BC, ptr [rdi + 2]);
	block->mov(DE, ptr [rdi + 4]);
	block->mov(HL, ptr [rdi + 6]);
	block->mov(PC, ptr [rdi + 8]);
	block->mov(SP, ptr [rdi + 10]);

	block->inc(PC);

	cpu->set_pc(cpu->get_pc() + 1);
	return false;
}

bool Amd64_Emitter :: instr_20(JitBlock *block, Cpu *cpu, Mmu *mmu) {

	int displacement = ((std::int8_t) mmu->read_byte(cpu->get_pc() + 1));

	std::cout << BOLDBLUE << "[JIT] JR NZ, " << displacement << RESET << "\n";

	block->add(PC, 2);
	
	// Test if al (F) has MSB set
	block->test(F, 0b10000000);

	// CF has the MSB of AL (Zero flag bit)
	block->jnz("@finish20");

	block->add(PC, displacement);
	
	block->jmp("@finish20");

	// Code for the "if" case (bit 7 is set)
	block->L("@finish20");
	
	return true;
}

bool Amd64_Emitter :: instr_21(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	std::cout << BOLDBLUE << "[JIT] LD HL, $" << format("{:02X}",
		(std::uint16_t) (((0xFF & mmu->read_byte(cpu->get_pc() + 2)) << 8) | (0xFF & mmu->read_byte(cpu->get_pc() + 1)))) << RESET << std::endl;
	
	block->mov(HL, (std::uint16_t) (((0xFF & mmu->read_byte(cpu->get_pc() + 2)) << 8) | (0xFF & mmu->read_byte(cpu->get_pc() + 1))));

	cpu->set_pc(cpu->get_pc() + 3);

	block->add(PC, 3);

	return false;
}

bool Amd64_Emitter :: instr_31(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	std::cout << BOLDBLUE << "[JIT] LD SP, $" << format("{:02X}",
		(std::uint16_t) (((0xFF & mmu->read_byte(cpu->get_pc() + 2)) << 8) | (0xFF & mmu->read_byte(cpu->get_pc() + 1)))) << RESET << std::endl;
	
	block->mov(SP, (std::uint16_t) (((0xFF & mmu->read_byte(cpu->get_pc() + 2)) << 8) | (0xFF & mmu->read_byte(cpu->get_pc() + 1))));

	cpu->set_pc(cpu->get_pc() + 3);
	block->add(PC, 3);
	return false;
}

bool Amd64_Emitter :: instr_32(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	std::cout << BOLDBLUE << "[JIT] LD (HL-), A" << RESET << std::endl;

	jit_save_frame(block, cpu, mmu);

	call_func_three_arg(block, mmu, &Mmu::write_byte_wrapper, reinterpret_cast<uintptr_t>(mmu), ah, cx);

	jit_restore_frame(block, cpu, mmu);

	block->dec(HL);

	cpu->set_pc(cpu->get_pc() + 1);
	block->inc(PC);

	return false;
}

bool Amd64_Emitter :: instr_3e(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	std::cout << BOLDBLUE << "[JIT] LD A, $" << format("{:02X}",
		(std::uint8_t) ((0xFF & mmu->read_byte(cpu->get_pc() + 1)))) << RESET << std::endl;
	
	block->mov(A, (std::uint8_t) ((0xFF & mmu->read_byte(cpu->get_pc() + 1))));

	cpu->set_pc(cpu->get_pc() + 2);
	block->add(PC, 2);
	return false;
}

bool Amd64_Emitter :: instr_4f(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	std::cout << BOLDBLUE << "[JIT] LD C, A" << RESET << std::endl;
	
	block->mov(C, A);

	cpu->set_pc(cpu->get_pc() + 1);
	block->inc(PC);
	return false;
}

bool Amd64_Emitter :: instr_77(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	std::cout << BOLDBLUE << "[JIT] LD (HL), A" << RESET << std::endl;

	jit_save_frame(block, cpu, mmu);

	call_func_three_arg(block, mmu, &Mmu::write_byte_wrapper, reinterpret_cast<uintptr_t>(mmu), ah, cx);

	jit_restore_frame(block, cpu, mmu);

	cpu->set_pc(cpu->get_pc() + 1);
	block->inc(PC);
	return false;
}

bool Amd64_Emitter :: instr_af(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	std::cout << BOLDBLUE << "[JIT] XOR A, A" << RESET << std::endl;
	
	block->xor_(A, A);

	block->test(A, A);

	block->jne("@skip_set_zero");

	block->or_(F, 0b10000000);
	
	block->L("@skip_set_zero");

	// Unset 6, 5 and 4
	block->and_(F, 0b10001111);
	
	cpu->set_pc(cpu->get_pc() + 1);
	block->inc(PC);
	return false;
}

bool Amd64_Emitter :: instr_e0(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	std::cout << BOLDBLUE << "[JIT] LD ($FF00+" << format("{:02X}", (std::uint8_t)
		((0xFF & mmu->read_byte(cpu->get_pc() + 1)))) << "), A" << RESET << std::endl;
	
	jit_save_frame(block, cpu, mmu);

	// Third argument to write_byte_wrapper
	block->movzx(edx, C);

	block->add(edx, (std::uint8_t) ((0xFF & mmu->read_byte(cpu->get_pc() + 1))));

	call_func_three_arg(block, mmu, &Mmu::write_byte_wrapper, reinterpret_cast<uintptr_t>(mmu), A, dx);

	jit_restore_frame(block, cpu, mmu);

	cpu->set_pc(cpu->get_pc() + 2);
	block->add(PC, 2);

	return false;
}
bool Amd64_Emitter :: instr_e2(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	std::cout << BOLDBLUE << "[JIT] LD ($FF00+C), A" << RESET << std::endl;
	
	jit_save_frame(block, cpu, mmu);

	block->movzx(edx, C);

	block->add(edx, 0xFF00);

	call_func_three_arg(block, mmu, &Mmu::write_byte_wrapper, reinterpret_cast<uintptr_t>(mmu), A, dx);

	jit_restore_frame(block, cpu, mmu);

	cpu->set_pc(cpu->get_pc() + 1);
	
	block->inc(PC);

	return false;
}

void debug(JitBlock *block)
{
	block->mov(rax, 60);

    block->mov(rdi, 0);

    block->syscall();
}

bool Amd64_Emitter :: instr_c5(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	std::cout << BOLDBLUE << "[JIT] PUSH BC" << RESET << std::endl;

	jit_save_frame(block, cpu, mmu);

	call_func_three_arg(block, mmu, &Mmu::write_byte_wrapper, reinterpret_cast<uintptr_t>(mmu), BC, SP);

	jit_restore_frame(block, cpu, mmu);

	jit_save_frame(block, cpu, mmu);

	block->shr(BC, 8);

	call_func_three_arg(block, mmu, &Mmu::write_byte_wrapper, reinterpret_cast<uintptr_t>(mmu), BC, SP);

	jit_restore_frame(block, cpu, mmu);

	block->sub(SP, 2);

	cpu->set_pc(cpu->get_pc() + 1);
	block->add(PC, 1);
	
	return false;
}

bool Amd64_Emitter :: instr_cb7c(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	std::cout << BOLDBLUE << "[JIT] BIT 7, H" << RESET << std::endl;

	// Test bit 7 of CH (Maps to H)
	block->test(H, 0b10000000);

	// CF has the MSB of H
	block->jz("@h_bit7_unset_zero_flag");
	block->and_(F, 0b01111111);

	// if (false) Set the flag
	block->jmp("@finish_bit7h");

	// if (true) Unset the flag
	block->L("@h_bit7_unset_zero_flag");
	block->or_(F, 0b10000000);

	block->L("@finish_bit7h");

	// Unset Negative bit (6) Flag in AL
	block->and_(F, 0b10111111);

	// Set Half Carry bit (5) Flag in AL
	block->or_(F, 0b00100000);

	cpu->set_pc(cpu->get_pc() + 1);
	block->inc(PC);

	return false;
}

bool Amd64_Emitter :: instr_cd(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	std::cout << BOLDBLUE << "[JIT] CALL $" << format("{:04X}",
		(std::uint16_t) (((0xFF & mmu->read_byte(cpu->get_pc() + 2)) << 8) | (0xFF & mmu->read_byte(cpu->get_pc() + 1)))) << RESET << std::endl;

	std::uint16_t newpc = (((0xFF & mmu->read_byte(cpu->get_pc() + 2)) << 8) | (0xFF & mmu->read_byte(cpu->get_pc() + 1)));

	jit_save_frame(block, cpu, mmu);
	
	call_func_three_arg(block, mmu, &Mmu::write_byte_wrapper, reinterpret_cast<uintptr_t>(mmu), r9b, SP);

	jit_restore_frame(block, cpu, mmu);

	jit_save_frame(block, cpu, mmu);

	block->mov(rdi, reinterpret_cast<uintptr_t>(mmu));

	block->shr(PC, 8);

	block->dec(SP);

	call_func_three_arg(block, mmu, &Mmu::write_byte_wrapper, reinterpret_cast<uintptr_t>(mmu), r9b, SP);

	jit_restore_frame(block, cpu, mmu);

	block->sub(SP, 2);

	cpu->set_pc(cpu->get_pc() + 1);
	block->mov(PC, newpc);
	
	return true;
}
