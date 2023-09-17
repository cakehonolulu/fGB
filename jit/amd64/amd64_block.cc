#include <jit/amd64/amd64_block.hh>

#if __has_include(<format>)
    #include <format>
    using std::format;
#else
    #include <fmt/format.h>
    using fmt::format;
#endif

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
