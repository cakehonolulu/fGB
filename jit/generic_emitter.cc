#include <jit/generic_emitter.hh>
#include <jit/generic_block.hh>
#include <mmu/mmu.hh>
#include <vector>
#include <xbyak/xbyak.h>
#include <fgb.hh>

Emitter :: Emitter(Mmu *mmu_) {
	mmu = mmu_;
}
