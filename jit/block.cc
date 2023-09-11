#include <jit/block.hh>

int current_id = 0;

JitBlock :: JitBlock()
{
	pc = 0xFFFF;
	id = current_id++;
	data = (uintptr_t *) new uint8_t[BLOCK_SIZE];
}

JitBlock :: JitBlock(uint8_t id_, uint16_t pc_)
	: pc(pc_), id(id_)
{
	data = (uintptr_t *) new uint8_t[BLOCK_SIZE];
}

JitBlock :: ~JitBlock() {
	delete data;
}
