#include <jit/block.hh>
#include <capstone/capstone.h>

int current_id = 0;

JitBlock :: JitBlock()
{
	pc = 0xFFFF;
	id = current_id++;
}

JitBlock :: JitBlock(uint8_t id_, uint16_t pc_)
	: pc(pc_), id(id_)
{
}

JitBlock :: ~JitBlock() {
}

int JitBlock :: execute() {
	typedef int (*FuncPtr)();
	FuncPtr func = (FuncPtr) getCode();
	return func();
}

size_t JitBlock :: get_size() {
	return getSize();
}

const uint8_t * JitBlock :: get_location() {
	return getCode();
}

void JitBlock :: disassemble() {
	// Create a Capstone disassembler instance
	csh handle;
	cs_insn* insn;
	size_t count;

	if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK) {
		std::cerr << "Failed to initialize Capstone" << std::endl;
		return;
	}

	// Get a pointer to the generated code from Xbyak
	const uint8_t* codePtr = get_location();
	size_t codeSize = get_size();

	// Disassemble the code
	count = cs_disasm(handle, codePtr, codeSize, 0, 0, &insn);

	if (count > 0) {
		for (size_t i = 0; i < count; i++) {
			printf("0x%" PRIx64 ": %s %s\n", insn[i].address, insn[i].mnemonic, insn[i].op_str);
		}
		
		cs_free(insn, count);
	} else {
		std::cerr << "Failed to disassemble code" << std::endl;
	}

	// Close the Capstone disassembler
	cs_close(&handle);
}
