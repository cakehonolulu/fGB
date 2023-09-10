#pragma once

#include "compiler.h"

#define EMIT_BYTE(byte) (block->data[block->current_len++] = byte)

#define EMIT_BYTES(bytes)										\
	({															\
		for (int byte = 0; byte < sizeof(bytes); byte++)		\
		{														\
			EMIT(bytes[byte]);									\
		}														\
	})

#define EMIT_QWORD(qword)										\
	({															\
		uintptr_t *loc = (uintptr_t *) (&block->data[block->current_len]);	\
		*loc = qword;											\
		block->current_len += sizeof(uintptr_t);				\
	})

enum x86_regs {
	rax, eax, ax, ah, al,
	rbx, ebx, bx, bh, bl,
	rcx, ecx, cx, ch, cl,
	rdx, edx, dx, dh, dl,
	rsi, esi, si,
	rdi, edi, di
};

#define movq_reg64_imm64(dst, src)								\
	({                                  						\
		uint8_t dst_;											\
																\
		switch (dst)											\
		{														\
			case rax: dst_ = 0xB8; break;						\
			case rbx: dst_ = 0xBB; break;						\
			case rcx: dst_ = 0xB9; break;						\
			case rdx: dst_ = 0xBA; break;						\
			case rsi: dst_ = 0xBE; break;						\
			case rdi: dst_ = 0xBF; break;						\
		}														\
																\
		EMIT_BYTE(0x48);										\
		EMIT_BYTE(dst_);										\
		EMIT_QWORD(src);										\
	})

#define movb_reg16_indirect_reg64_offset(dst, src, off)			\
	({															\
		uint8_t src_, dst_;										\
																\
		switch (src)											\
		{														\
			case rax: src_ = 0b000; break;						\
			case rbx: src_ = 0b011; break;						\
			case rcx: src_ = 0b001; break;						\
			case rdx: src_ = 0b010; break;						\
			case rsi: src_ = 0b110; break;						\
			case rdi: src_ = 0b111; break;						\
		}														\
																\
		switch (dst)											\
		{														\
			case ax: dst_ = 0b000; break;						\
			case bx: dst_ = 0b011; break;						\
			case cx: dst_ = 0b001; break;						\
			case dx: dst_ = 0b010; break;						\
			case si: dst_ = 0b110; break;						\
			case di: dst_ = 0b111; break;						\
		}														\
																\
		EMIT_BYTE(0x66);										\
		EMIT_BYTE(0x8B);										\
																\
		/* Craft Mod/RM Byte */									\
    	EMIT_BYTE((0b01 << 6) | (dst_ << 3) | (src_));			\
		EMIT_BYTE(off);											\
	})

/*
#define movb_reg_regoff(dst, src)								\
	({															\

	})*/