#include <jit/compiler.hh>

Compiler :: Compiler()
{
	blocks = new JitBlock[BLOCK_COUNT];
    used_blocks = 0;
}

Compiler :: ~Compiler() {
	delete blocks;
}
