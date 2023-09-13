#include <jit/block.hh>
#include <capstone/capstone.h>
#include <fgb.hh>

#if __has_include(<format>)
    #include <format>
    using std::format;
#else
    #include <fmt/format.h>
    using fmt::format;
#endif

JitBlock :: JitBlock()
{
	pc = 0xFFFF;
	id = (uint16_t) rand();
	dirty = false;
}

JitBlock :: JitBlock(uint8_t id_, uint16_t pc_)
	: pc(pc_), id(id_)
{
}

JitBlock :: ~JitBlock() {
}

bool JitBlock :: is_dirty() {
	return dirty;
}

void JitBlock :: set_dirty(bool dirty_) {
	dirty = dirty_;
}

std::uint16_t JitBlock :: get_pc() {
	return pc;
}

void JitBlock :: set_pc(std::uint16_t pc_) {
	pc = pc_;
}

std::uint16_t JitBlock :: get_id() {
	return id;
}

void JitBlock :: set_id(std::uint16_t id_) {
	id = id_;
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

	std::cout << "\n-------- BLOCK  INFORMATION --------\n";
	std::cout << "     Block ID: " << get_id() << ", PC: 0x" << format("{:04X}", get_pc()) << "\n";
	std::cout << "------------------------------------\n";

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

	std::cout << "------------------------------------\n\n";

	// Close the Capstone disassembler
	cs_close(&handle);
}
