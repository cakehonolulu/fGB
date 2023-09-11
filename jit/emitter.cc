#include <jit/emitter.hh>
#include <jit/block.hh>
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

void Emitter :: jit_compile_block(JitBlock *block, Cpu *cpu, std::vector<char> *bootrom) {

    std::cout << "[" << BOLDBLUE << "*" << RESET << "] Emitting instructions...\n";

	block->set_dirty(true);
	block->set_pc(cpu->get_pc());

	bool branch_found = false;

    jit_emit_prologue(block, cpu, bootrom);

	std::uint8_t instruction;

	while (!branch_found)
	{
		instruction = bootrom->at(cpu->get_pc());

    	branch_found = jit_process_opcode(instruction, block, cpu, bootrom);
	}

	jit_emit_epilogue(block, cpu, bootrom);


    block->disassemble();
}


bool Emitter :: jit_process_opcode(std::uint8_t opcode, JitBlock *block, Cpu *cpu, std::vector<char> *bootrom) {
    bool branch_found = false;

	Instruction_ func = instructions[opcode].func;

    if (func != NULL)
    {
        branch_found = (this->*instructions[opcode].func)(block, cpu, bootrom);
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


bool Emitter :: jit_process_extended_opcode(JitBlock *block, Cpu *cpu, std::vector<char> *bootrom) {
	bool branch_found = false;

	cpu->set_pc(cpu->get_pc() + 1);

	Instruction_ func = extended_instructions[bootrom->at(cpu->get_pc())].func;
	
    if (func != NULL)
    {	
        branch_found = (this->*extended_instructions[bootrom->at(cpu->get_pc())].func)(block, cpu, bootrom);
    }
	else
	{
        std::cout << BOLDRED << "execute: Extended Instruction " << format("{:#04x}", bootrom->at(cpu->get_pc())) << " unimplemented!" << RESET << std::endl;
		
		block->disassemble();

		cpu->print_regs();

		exit(1);
    }

    return branch_found;
}


void Emitter :: jit_emit_prologue(JitBlock *block, Cpu *cpu, std::vector<char> *bootrom) {
    /*
		Register Allocation (x86_64):

		Host                    Target
		AX                      AF
		BX                      BC
		CX                      HL
		DX						PC
		R8.W					SP
		R9.B					Flags
	*/

	// Map target registers to host registers if block is not dirty

	// mov rdi, &cpu
	block->mov(rdi, (size_t)cpu);

	// AF
	block->mov(ax, ptr [rdi]);

	// BC
	block->mov(bx, ptr [rdi + 2]);

	// HL
	block->mov(cx, ptr [rdi + 4]);

	// PC
	block->mov(dx, ptr [rdi + 6]);

	// SP
	block->mov(r8w, ptr [rdi + 8]);

	// Flags
	block->mov(r9b, ptr [rdi + 10]);
}

void Emitter :: jit_emit_epilogue(JitBlock *block, Cpu *cpu, std::vector<char> *bootrom) {
    /*
		Register Allocation (x86_64):

		Host                    Target
		AX                      AF
		BX                      BC
		CX                      HL
		DX						PC
		R8.W					SP
		R9.B					Flags
	*/

	// Map target registers to host registers if block is not dirty

	// mov rdi, &cpu
	block->mov(rdi, (size_t)cpu);

	// AF
	block->mov(ptr [rdi], ax);

	// BC
	block->mov(ptr [rdi + 2], bx);

	// HL
	block->mov(ptr [rdi + 4], cx);

	// PC
	block->mov(ptr [rdi + 6], dx);

	// SP
	block->mov(ptr [rdi + 8], r8w);

	// Flags
	block->mov(ptr [rdi + 10], r9b);

	// RET
	block->ret();
}

bool Emitter :: instr_20(JitBlock *block, Cpu *cpu, std::vector<char> *bootrom) {
	int displacement = (bootrom->at(cpu->get_pc() + 1));

	std::cout << BOLDBLUE << "[JIT] JR NZ, " << displacement << RESET << "\n";

	cpu->set_pc(cpu->get_pc() + 2);
	cpu->set_pc(cpu->get_pc() + displacement);
	
	return true;
}

bool Emitter :: instr_21(JitBlock *block, Cpu *cpu, std::vector<char> *bootrom) {
	std::cout << BOLDBLUE << "[JIT] LD HL, " << format("{:#02x}",
		(std::uint16_t) (((bootrom->at(cpu->get_pc() + 2)) << 8) | (bootrom->at(cpu->get_pc() + 1)))) << RESET << std::endl;
	
	block->mov(cx, (std::uint16_t) (((bootrom->at(cpu->get_pc() + 2)) << 8) | (bootrom->at(cpu->get_pc() + 1))));

	cpu->set_pc(cpu->get_pc() + 3);
	return false;
}

bool Emitter :: instr_31(JitBlock *block, Cpu *cpu, std::vector<char> *bootrom) {
	std::cout << BOLDBLUE << "[JIT] LD SP, " << format("{:#02x}",
		(std::uint16_t) (((bootrom->at(cpu->get_pc() + 2)) << 8) | (bootrom->at(cpu->get_pc() + 1)))) << RESET << std::endl;
	
	block->mov(r8w, (std::uint16_t) (((bootrom->at(cpu->get_pc() + 2)) << 8) | (bootrom->at(cpu->get_pc() + 1))));

	cpu->set_pc(cpu->get_pc() + 3);
	return false;
}

bool Emitter :: instr_32(JitBlock *block, Cpu *cpu, std::vector<char> *bootrom) {
	std::cout << BOLDBLUE << "[JIT] LD (HL-), A" << RESET << std::endl;

	block->dec(cx);

	cpu->set_pc(cpu->get_pc() + 1);
	return false;
}

bool Emitter :: instr_af(JitBlock *block, Cpu *cpu, std::vector<char> *bootrom) {
	std::cout << BOLDBLUE << "[JIT] XOR A, A" << RESET << std::endl;
	
	block->xor_(ah, ah);

	cpu->set_pc(cpu->get_pc() + 1);
	return false;
}


bool Emitter :: instr_cb7c(JitBlock *block, Cpu *cpu, std::vector<char> *bootrom) {
	std::cout << BOLDBLUE << "[JIT] BIT 7, H" << RESET << std::endl;
	
	cpu->set_pc(cpu->get_pc() + 1);
	return false;
}

