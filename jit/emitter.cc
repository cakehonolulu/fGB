#include <jit/emitter.hh>
#include <jit/block.hh>
#include <mmu.hh>
#include <vector>
#include <xbyak/xbyak.h>
#include <fgb.hh>

using namespace Xbyak;
using namespace Xbyak::util;

#if __has_include(<format>)
    #include <format>
    using std::format;
#else
    #include <fmt/format.h>
    using fmt::format;
#endif

Emitter :: Emitter(Mmu *mmu_) {
	mmu = mmu_;
}

void Emitter :: jit_compile_block(JitBlock *block, Cpu *cpu, Mmu *mmu) {

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

	cpu->print_regs();
}


bool Emitter :: jit_process_opcode(std::uint8_t opcode, JitBlock *block, Cpu *cpu, Mmu *mmu) {
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


bool Emitter :: jit_process_extended_opcode(JitBlock *block, Cpu *cpu, Mmu *mmu) {
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


void Emitter :: jit_emit_prologue(JitBlock *block, Cpu *cpu, Mmu *mmu) {
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
	block->mov(ax, ptr [rdi]);

	// BC
	block->mov(bx, ptr [rdi + 2]);

	// DE
	block->mov(dx, ptr [rdi + 4]);

	// HL
	block->mov(cx, ptr [rdi + 6]);

	// PC
	block->mov(r9w, ptr [rdi + 8]);

	// SP
	block->mov(r8w, ptr [rdi + 10]);
}

void Emitter :: jit_emit_epilogue(JitBlock *block, Cpu *cpu, Mmu *mmu) {
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
	block->mov(ptr [rdi], ax);

	// BC
	block->mov(ptr [rdi + 2], bx);

	// DE
	block->mov(ptr [rdi + 4], dx);

	// HL
	block->mov(ptr [rdi + 6], cx);

	// PC
	block->mov(ptr [rdi + 8], r9w);

	// SP
	block->mov(ptr [rdi + 10], r8w);

	// RET
	block->ret();
}

void Emitter :: jit_restore_frame(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	block->mov(rdi, (size_t)cpu);
	block->mov(ax, ptr [rdi]);
	block->mov(bx, ptr [rdi + 2]);
	block->mov(dx, ptr [rdi + 4]);
	block->mov(cx, ptr [rdi + 6]);
	block->mov(r9w, ptr [rdi + 8]);
	block->mov(r8w, ptr [rdi + 10]);
}

void Emitter :: jit_save_frame(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	block->mov(rdi, (size_t)cpu);
	block->mov(ptr [rdi], ax);
	block->mov(ptr [rdi + 2], bx);
	block->mov(ptr [rdi + 4], dx);
	block->mov(ptr [rdi + 6], cx);
	block->mov(ptr [rdi + 8], r9w);
	block->mov(ptr [rdi + 10], r8w);
}

bool Emitter :: instr_0c(JitBlock *block, Cpu *cpu, Mmu *mmu) {

	std::cout << BOLDBLUE << "[JIT] INC C" << RESET << "\n";

	block->inc(bl);

	block->cmp(bl, 0);

	block->je("@0c_set_zero_flag");

	// Unset Z bit
	block->and_(al, 0x7F);

	block->jmp("@0c_check_half_carry");

	block->L("@0c_set_zero_flag");

	// Set Z bit
	block->or_(al, 0x80);

	block->L("@0c_check_half_carry");

	// Test if bit 9 is set on BC
	block->test(bx, 0x0200);

	block->jnz("@0c_hc_bit_is_set");   // Jump if the 9th bit is set
	
	block->and_(al, 0xDF);

	block->jmp("@0c_finish");

	block->L("@0c_hc_bit_is_set");

	block->or_(al, 0x20);

	block->L("@0c_finish");

	block->inc(r9w);
	cpu->set_pc(cpu->get_pc() + 1);

	return false;
}

bool Emitter :: instr_0e(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	std::int8_t d8 = (mmu->read_byte(cpu->get_pc() + 1));

	std::cout << BOLDBLUE << "[JIT] LD C, $" << format("{:02X}", d8) << RESET << "\n";

	block->mov(bl, d8);

	block->add(r9w, 2);

	cpu->set_pc(cpu->get_pc() + 2);

	return false;
}

bool Emitter :: instr_11(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	std::cout << BOLDBLUE << "[JIT] LD DE, $" << format("{:04X}",
		(std::uint16_t) (((0xFF & mmu->read_byte(cpu->get_pc() + 2)) << 8) | (0xFF & mmu->read_byte(cpu->get_pc() + 1)))) << RESET << std::endl;
	
	block->mov(dx, (std::uint16_t) (((0xFF & mmu->read_byte(cpu->get_pc() + 2)) << 8) | (0xFF & mmu->read_byte(cpu->get_pc() + 1))));

	cpu->set_pc(cpu->get_pc() + 3);

	block->add(r9w, 3);
	return false;
}

bool Emitter :: instr_1a(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	std::cout << BOLDBLUE << "[JIT] LD A, (DE)" << std::endl;
	
	jit_save_frame(block, cpu, mmu);

	// Call the function using System-V x86_64 ABI
	// First argument to read_byte_wrapper
	block->mov(rdi, reinterpret_cast<uintptr_t>(mmu));

	// Second argument to read_byte_wrapper
	block->movzx(esi, dx);

	block->mov(rax, reinterpret_cast<uintptr_t>(&Mmu::read_byte_wrapper));

	block->call(rax);

	block->mov(rdi, (size_t)cpu);
	block->mov(al, ptr [rdi]);
	block->mov(bx, ptr [rdi + 2]);
	block->mov(dx, ptr [rdi + 4]);
	block->mov(cx, ptr [rdi + 6]);
	block->mov(r9w, ptr [rdi + 8]);
	block->mov(r8w, ptr [rdi + 10]);

	block->inc(r9w);
	cpu->set_pc(cpu->get_pc() + 1);
	return false;
}

bool Emitter :: instr_20(JitBlock *block, Cpu *cpu, Mmu *mmu) {

	int displacement = ((std::int8_t) mmu->read_byte(cpu->get_pc() + 1));

	std::cout << BOLDBLUE << "[JIT] JR NZ, " << displacement << RESET << "\n";

	block->add(r9w, 2);
	
	// Test if al (F) has MSB set
	block->test(al, 0b10000000);

	// CF has the MSB of AL (Zero flag bit)
	block->jnz("@finish20");

	block->add(r9w, displacement);
	
	block->jmp("@finish20");


	// Code for the "if" case (bit 7 is set)
	block->L("@finish20");

	
	return true;
}

bool Emitter :: instr_21(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	std::cout << BOLDBLUE << "[JIT] LD HL, $" << format("{:02X}",
		(std::uint16_t) (((0xFF & mmu->read_byte(cpu->get_pc() + 2)) << 8) | (0xFF & mmu->read_byte(cpu->get_pc() + 1)))) << RESET << std::endl;
	
	block->mov(cx, (std::uint16_t) (((0xFF & mmu->read_byte(cpu->get_pc() + 2)) << 8) | (0xFF & mmu->read_byte(cpu->get_pc() + 1))));

	cpu->set_pc(cpu->get_pc() + 3);

	block->add(r9w, 3);
	return false;
}

bool Emitter :: instr_31(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	std::cout << BOLDBLUE << "[JIT] LD SP, $" << format("{:02X}",
		(std::uint16_t) (((0xFF & mmu->read_byte(cpu->get_pc() + 2)) << 8) | (0xFF & mmu->read_byte(cpu->get_pc() + 1)))) << RESET << std::endl;
	
	block->mov(r8w, (std::uint16_t) (((0xFF & mmu->read_byte(cpu->get_pc() + 2)) << 8) | (0xFF & mmu->read_byte(cpu->get_pc() + 1))));

	cpu->set_pc(cpu->get_pc() + 3);
	block->add(r9w, 3);
	return false;
}

bool Emitter :: instr_32(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	std::cout << BOLDBLUE << "[JIT] LD (HL-), A" << RESET << std::endl;

	jit_save_frame(block, cpu, mmu);

	// Call the function using System-V x86_64 ABI
	// First argument to write_byte_wrapper
	block->mov(rdi, reinterpret_cast<uintptr_t>(mmu));

	// Second argument to write_byte_wrapper
	block->movzx(esi, ah);

	// Third argument to write_byte_wrapper
	block->movzx(rdx, cx);

	block->mov(rax, reinterpret_cast<uintptr_t>(&Mmu::write_byte_wrapper));

	block->call(rax);

	jit_restore_frame(block, cpu, mmu);

	block->dec(cx);

	cpu->set_pc(cpu->get_pc() + 1);
	block->inc(r9w);
	return false;
}

bool Emitter :: instr_3e(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	std::cout << BOLDBLUE << "[JIT] LD A, $" << format("{:02X}",
		(std::uint8_t) ((0xFF & mmu->read_byte(cpu->get_pc() + 1)))) << RESET << std::endl;
	
	block->mov(ah, (std::uint8_t) ((0xFF & mmu->read_byte(cpu->get_pc() + 1))));

	cpu->set_pc(cpu->get_pc() + 2);
	block->add(r9w, 2);
	return false;
}

bool Emitter :: instr_77(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	std::cout << BOLDBLUE << "[JIT] LD (HL), A" << RESET << std::endl;

	jit_save_frame(block, cpu, mmu);

	// Call the function using System-V x86_64 ABI
	// First argument to write_byte_wrapper
	block->mov(rdi, reinterpret_cast<uintptr_t>(mmu));

	// Second argument to write_byte_wrapper
	block->movzx(esi, ah);

	// Third argument to write_byte_wrapper
	block->movzx(rdx, cx);

	block->mov(rax, reinterpret_cast<uintptr_t>(&Mmu::write_byte_wrapper));

	block->call(rax);

	jit_restore_frame(block, cpu, mmu);

	cpu->set_pc(cpu->get_pc() + 1);
	block->inc(r9w);
	return false;
}

bool Emitter :: instr_af(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	std::cout << BOLDBLUE << "[JIT] XOR A, A" << RESET << std::endl;
	
	block->xor_(ah, ah);

	block->test(ah, ah);

	block->jne("@skip_set_zero");

	block->or_(al, 0b10000000);
	
	block->L("@skip_set_zero");

	// Unset 6, 5 and 4
	block->and_(al, 0b10001111);
	
	cpu->set_pc(cpu->get_pc() + 1);
	block->inc(r9w);
	return false;
}

bool Emitter :: instr_e0(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	std::cout << BOLDBLUE << "[JIT] LD ($FF00+" << format("{:02X}", (std::uint8_t)
		((0xFF & mmu->read_byte(cpu->get_pc() + 1)))) << "), A" << RESET << std::endl;
	
	jit_save_frame(block, cpu, mmu);

	// Call the function using System-V x86_64 ABI
	// First argument to write_byte_wrapper
	block->mov(rdi, reinterpret_cast<uintptr_t>(mmu));

	// Second argument to write_byte_wrapper
	block->movzx(esi, ah);

	// Third argument to write_byte_wrapper
	block->movzx(edx, ch);

	block->add(edx, (std::uint8_t) ((0xFF & mmu->read_byte(cpu->get_pc() + 1))));

	block->mov(rax, reinterpret_cast<uintptr_t>(&Mmu::write_byte_wrapper));

	block->call(rax);

	jit_restore_frame(block, cpu, mmu);

	cpu->set_pc(cpu->get_pc() + 2);
	block->add(r9w, 2);

	return false;
}
bool Emitter :: instr_e2(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	std::cout << BOLDBLUE << "[JIT] LD ($FF00+C), A" << RESET << std::endl;
	

	jit_save_frame(block, cpu, mmu);

	// Call the function using System-V x86_64 ABI
	// First argument to write_byte_wrapper
	block->mov(rdi, reinterpret_cast<uintptr_t>(mmu));

	// Second argument to write_byte_wrapper
	block->movzx(esi, ah);

	// Third argument to write_byte_wrapper
	block->movzx(edx, ch);

	block->add(edx, 0xFF00);

	block->mov(rax, reinterpret_cast<uintptr_t>(&Mmu::write_byte_wrapper));

	block->call(rax);

	jit_restore_frame(block, cpu, mmu);

	cpu->set_pc(cpu->get_pc() + 1);
	block->inc(r9w);

	return false;
}

void debug(JitBlock *block)
{
	block->mov(rax, 60);

    block->mov(rdi, 0);

    block->syscall();
}

bool Emitter :: instr_cb7c(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	std::cout << BOLDBLUE << "[JIT] BIT 7, H" << RESET << std::endl;

	// Test bit 7 of CH (Maps to H)
	block->test(ch, 0b10000000);

	// CF has the MSB of H
	block->jz("@h_bit7_unset_zero_flag");
	block->and_(al, 0b01111111);

	// if (false) Set the flag
	block->jmp("@finish_bit7h");

	// if (true) Unset the flag
	block->L("@h_bit7_unset_zero_flag");
	block->or_(al, 0b10000000);

	block->L("@finish_bit7h");

	// Unset Negative bit (6) Flag in AL
	block->and_(al, 0b10111111);

	// Set Half Carry bit (5) Flag in AL
	block->or_(al, 0b00100000);

	cpu->set_pc(cpu->get_pc() + 1);
	block->inc(r9w);

	return false;
}

bool Emitter :: instr_cd(JitBlock *block, Cpu *cpu, Mmu *mmu) {
	std::cout << BOLDBLUE << "[JIT] CALL $" << format("{:04X}",
		(std::uint16_t) (((0xFF & mmu->read_byte(cpu->get_pc() + 2)) << 8) | (0xFF & mmu->read_byte(cpu->get_pc() + 1)))) << RESET << std::endl;

	std::uint16_t newpc = (((0xFF & mmu->read_byte(cpu->get_pc() + 2)) << 8) | (0xFF & mmu->read_byte(cpu->get_pc() + 1)));

	jit_save_frame(block, cpu, mmu);

	// Call the function using System-V x86_64 ABI
	// First argument to write_byte_wrapper
	block->mov(rdi, reinterpret_cast<uintptr_t>(mmu));

	// Second argument to write_byte_wrapper
	block->movzx(esi, r9b);

	// Third argument to write_byte_wrapper
	block->movzx(rdx, r8w);

	block->mov(rax, reinterpret_cast<uintptr_t>(&Mmu::write_byte_wrapper));

	block->call(rax);

	jit_restore_frame(block, cpu, mmu);

	jit_save_frame(block, cpu, mmu);

	// Call the function using System-V x86_64 ABI
	// First argument to write_byte_wrapper
	block->mov(rdi, reinterpret_cast<uintptr_t>(mmu));

	block->shr(r9w, 8);

	// Second argument to write_byte_wrapper
	block->movzx(esi, r9b);

	block->dec(r8w);

	// Third argument to write_byte_wrapper
	block->movzx(rdx, r8w);

	block->mov(rax, reinterpret_cast<uintptr_t>(&Mmu::write_byte_wrapper));

	block->call(rax);

	jit_restore_frame(block, cpu, mmu);

	block->dec(r8w);
	block->dec(r8w);


	cpu->set_pc(cpu->get_pc() + 1);
	block->mov(r9w, newpc);
	
	return true;
}
