#pragma once

#include <jit/block.hh>
#include <mmu.hh>
#include <cpu.hh>
#include <cstdint>
#include <vector>

#define A   ah
#define F   al
#define AF  ax

#define B   bh
#define C   bl
#define BC  bx

#define D   dh
#define E   dl
#define DE  dx

#define H   ch
#define L_  cl
#define HL  cx

#define PC  r9w
#define SP  r8w

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
/* Suitable for calling under MS ABI */
#if _WIN32 || _WIN64
    #if _WIN64
// 64-bit
#define arg1q    rcx
#define arg2q    rdx
#define arg3q    r8
    #else
        #define NO_64BIT
    #endif
#endif

// 32-bit
#define arg1d    ecx
#define arg2d    edx
#define arg3d    r8d
// 16-bit
#define arg1w    cx
#define arg2w    dx
#define arg3w    r8w
// 8-bit
#define arg1b    cl
#define arg2b    dl
#define arg3b    r8b
#else
/* Suitable for calling under SysV ABI */
#if __GNUC__
    #if __x86_64__ || __ppc64__
// 64-bit
#define arg1q    rdi
#define arg2q    rsi
#define arg3q    rdx
    #else
        #define NO_64BIT
    #endif
#endif

// 32-bit
#define arg1d    edi
#define arg2d    esi
#define arg3d    edx
// 16-bit
#define arg1w    di
#define arg2w    si
#define arg3w    dx
// 8-bit
#define arg1b    dil
#define arg2b    sil
#define arg3b    dl
#endif

class Emitter {

    typedef bool (Emitter::*Instruction_)(JitBlock *, Cpu *, Mmu *);

    struct Instruction {
        Instruction_ func;
    };

public:

    const Instruction instructions[256] = {
        /*          +0	      +1	       +2	        +3	      +4	    +5	      +6	    +7	      +8	    +9	      +A	    +B	         +C	       +D	     +E	       +F      */
        /* 00+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { &Emitter::instr_06 }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { &Emitter::instr_0c }, { NULL }, { &Emitter::instr_0e }, { NULL     },
        /* 10+ */ { NULL }, { &Emitter::instr_11     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { &Emitter::instr_1a }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* 20+ */ { &Emitter::instr_20 }, { &Emitter::instr_21 }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* 30+ */ { NULL }, { &Emitter::instr_31 }, { &Emitter::instr_32 }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { &Emitter::instr_3e }, { NULL     },
        /* 40+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { &Emitter::instr_4f     },
        /* 50+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* 60+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* 70+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { &Emitter::instr_77 }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* 80+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* 90+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* A0+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { &Emitter::instr_af },
        /* B0+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL     },
        /* C0+ */ { NULL }, { NULL     }, { NULL     }, { NULL }, { NULL }, { &Emitter::instr_c5 }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { &Emitter::jit_process_extended_opcode    }, { NULL }, { &Emitter::instr_cd }, { NULL }, { NULL     },
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

    void handle_movz_arg(JitBlock* block, const Xbyak::Reg8& arg, std::uint8_t pos);

    void handle_movz_arg(JitBlock* block, const Xbyak::Reg16& arg, std::uint8_t pos);

    void handle_movz_arg(JitBlock* block, const Xbyak::Reg32& arg, std::uint8_t pos);

    template <typename FuncType, typename Arg1Type>
    void call_func_one_arg(JitBlock* block, Mmu* mmu, FuncType func, const Arg1Type& arg1);

    template <typename FuncType, typename Arg1Type, typename Arg2Type>
    void call_func_two_arg(JitBlock* block, Mmu* mmu, FuncType func, const Arg1Type& arg1, const Arg2Type& arg2);

    template <typename FuncType, typename Arg1Type, typename Arg2Type, typename Arg3Type>
    void call_func_three_arg(JitBlock* block, Mmu* mmu, FuncType func, const Arg1Type& arg1, const Arg2Type& arg2, const Arg3Type& arg3);

    bool instr_06(JitBlock *block, Cpu *cpu, Mmu *mmu);
    bool instr_0c(JitBlock *block, Cpu *cpu, Mmu *mmu);
    bool instr_0e(JitBlock *block, Cpu *cpu, Mmu *mmu);
    bool instr_11(JitBlock *block, Cpu *cpu, Mmu *mmu);
    bool instr_1a(JitBlock *block, Cpu *cpu, Mmu *mmu);
    bool instr_20(JitBlock *block, Cpu *cpu, Mmu *mmu);
    bool instr_21(JitBlock *block, Cpu *cpu, Mmu *mmu);
    bool instr_31(JitBlock *block, Cpu *cpu, Mmu *mmu);
    bool instr_32(JitBlock *block, Cpu *cpu, Mmu *mmu);
    bool instr_3e(JitBlock *block, Cpu *cpu, Mmu *mmu);
    bool instr_4f(JitBlock *block, Cpu *cpu, Mmu *mmu);
    bool instr_77(JitBlock *block, Cpu *cpu, Mmu *mmu);
    bool instr_af(JitBlock *block, Cpu *cpu, Mmu *mmu);
    bool instr_e0(JitBlock *block, Cpu *cpu, Mmu *mmu);
    bool instr_e2(JitBlock *block, Cpu *cpu, Mmu *mmu);
    bool instr_c5(JitBlock *block, Cpu *cpu, Mmu *mmu);
    bool instr_cb7c(JitBlock *block, Cpu *cpu, Mmu *mmu);
    bool instr_cd(JitBlock *block, Cpu *cpu, Mmu *mmu);
};
