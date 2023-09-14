#pragma once

#include <jit/block.hh>
#include <mmu.hh>
#include <cpu.hh>
#include <cstdint>
#include <vector>

class Emitter {

    typedef bool (Emitter::*Instruction_)(JitBlock *, Cpu *, Mmu *);

    struct Instruction {
        Instruction_ func;
    };

public:

    const Instruction instructions[256] = {
        /*          +0	      +1	       +2	        +3	      +4	    +5	      +6	    +7	      +8	    +9	      +A	    +B	         +C	       +D	     +E	       +F      */
        /* 00+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { &Emitter::instr_0c }, { NULL }, { &Emitter::instr_0e }, { NULL     },
        /* 10+ */ { NULL }, { &Emitter::instr_11     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { &Emitter::instr_1a }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* 20+ */ { &Emitter::instr_20 }, { &Emitter::instr_21 }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* 30+ */ { NULL }, { &Emitter::instr_31 }, { &Emitter::instr_32 }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { &Emitter::instr_3e }, { NULL     },
        /* 40+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* 50+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* 60+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* 70+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { &Emitter::instr_77 }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* 80+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* 90+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* A0+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { &Emitter::instr_af },
        /* B0+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* C0+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { &Emitter::jit_process_extended_opcode    }, { NULL }, { &Emitter::instr_cd }, { NULL }, { NULL     },
        /* D0+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* E0+ */ { &Emitter::instr_e0 }, { NULL     }, { &Emitter::instr_e2     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* F0+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     }
    };
    
    const Instruction extended_instructions[256] = {
        /*          +0	      +1	       +2	        +3	      +4	    +5	      +6	    +7	      +8	    +9	      +A	    +B	         +C	       +D	     +E	       +F      */
        /* 00+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* 10+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* 20+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* 30+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* 40+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* 50+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* 60+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* 70+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { &Emitter::instr_cb7c }, { NULL }, { NULL }, { NULL     },
        /* 80+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* 90+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* A0+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* B0+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* C0+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* D0+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* E0+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* F0+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     }
    };
    
    Mmu *mmu;

    Emitter(Mmu* mmu_);

    void jit_compile_block(JitBlock *block, Cpu *cpu, Mmu *mmu);

    void jit_emit_prologue(JitBlock *block, Cpu *cpu, Mmu *mmu);

    void jit_emit_epilogue(JitBlock *block, Cpu *cpu, Mmu *mmu);

    bool jit_process_opcode(std::uint8_t opcode, JitBlock *block, Cpu *cpu, Mmu *mmu);

    bool jit_process_extended_opcode(JitBlock *block, Cpu *cpu, Mmu *mmu);

    void jit_restore_frame(JitBlock *block, Cpu *cpu, Mmu *mmu);
    void jit_save_frame(JitBlock *block, Cpu *cpu, Mmu *mmu);


    bool instr_0c(JitBlock *block, Cpu *cpu, Mmu *mmu);
    bool instr_0e(JitBlock *block, Cpu *cpu, Mmu *mmu);
    bool instr_11(JitBlock *block, Cpu *cpu, Mmu *mmu);
    bool instr_1a(JitBlock *block, Cpu *cpu, Mmu *mmu);
    bool instr_20(JitBlock *block, Cpu *cpu, Mmu *mmu);
    bool instr_21(JitBlock *block, Cpu *cpu, Mmu *mmu);
    bool instr_31(JitBlock *block, Cpu *cpu, Mmu *mmu);
    bool instr_32(JitBlock *block, Cpu *cpu, Mmu *mmu);
    bool instr_3e(JitBlock *block, Cpu *cpu, Mmu *mmu);
    bool instr_77(JitBlock *block, Cpu *cpu, Mmu *mmu);
    bool instr_af(JitBlock *block, Cpu *cpu, Mmu *mmu);
    bool instr_e0(JitBlock *block, Cpu *cpu, Mmu *mmu);
    bool instr_e2(JitBlock *block, Cpu *cpu, Mmu *mmu);
    bool instr_cb7c(JitBlock *block, Cpu *cpu, Mmu *mmu);
    bool instr_cd(JitBlock *block, Cpu *cpu, Mmu *mmu);
};
