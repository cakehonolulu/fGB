#pragma once

#include <jit/generic_block.hh>
#include <mmu/mmu.hh>
#include <cpu/cpu.hh>
#include <cstdint>
#include <vector>

class Emitter {

public:

    Mmu *mmu;

    Emitter(Mmu* mmu_);

};
