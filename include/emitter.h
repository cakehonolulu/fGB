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
		uintptr_t *loc =										\
			(uintptr_t *) (&block->data[block->current_len]);	\
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

#define RET() ({ EMIT_BYTE(0xC3); })

#define movb_indirect_reg64_reg16(dst, src)						\
	({                                  						\
		uint8_t src_, dst_;										\
																\
		switch (src)											\
		{														\
			case ax: src_ = 0b000; break;						\
			case bx: src_ = 0b011; break;						\
			case cx: src_ = 0b001; break;						\
			case dx: src_ = 0b010; break;						\
			case si: src_ = 0b110; break;						\
			case di: src_ = 0b111; break;						\
		}														\
																\
		switch (dst)											\
		{														\
			case rax: dst_ = 0b000; break;						\
			case rbx: dst_ = 0b011; break;						\
			case rcx: dst_ = 0b001; break;						\
			case rdx: dst_ = 0b010; break;						\
			case rsi: dst_ = 0b110; break;						\
			case rdi: dst_ = 0b111; break;						\
		}														\
																\
		EMIT_BYTE(0x66);										\
		EMIT_BYTE(0x89);										\
																\
		/* Craft Mod/RM Byte */									\
    	EMIT_BYTE((0b00 << 6) | (src_ << 3) | (dst_));			\
	})

#define movq_reg64_imm64(dst, src)								\
	({                                  						\
		uint8_t dst_;											\
																\
		static_assert(dst != rax,								\
			"mov rax, [reg] has a specialized macro!");			\
																\
		switch (dst)											\
		{														\
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

#define movb_indirect_reg64_offset_reg16(dst, off, src)			\
	({															\
		uint8_t src_, dst_;										\
																\
		switch (src)											\
		{														\
			case ax: src_ = 0b000; break;						\
			case bx: src_ = 0b011; break;						\
			case cx: src_ = 0b001; break;						\
			case dx: src_ = 0b010; break;						\
			case si: src_ = 0b110; break;						\
			case di: src_ = 0b111; break;						\
		}														\
																\
		switch (dst)											\
		{														\
			case rax: dst_ = 0b000; break;						\
			case rbx: dst_ = 0b011; break;						\
			case rcx: dst_ = 0b001; break;						\
			case rdx: dst_ = 0b010; break;						\
			case rsi: dst_ = 0b110; break;						\
			case rdi: dst_ = 0b111; break;						\
		}														\
																\
		EMIT_BYTE(0x66);										\
		EMIT_BYTE(0x89);										\
																\
		/* Craft Mod/RM Byte */									\
    	EMIT_BYTE((0b01 << 6) | (src_ << 3) | (dst_));			\
		EMIT_BYTE(off);											\
	})

#define movw_indirect_reg64_offset_reg32(dst, off, src)			\
	({															\
		uint8_t src_, dst_;										\
																\
		switch (src)											\
		{														\
			case eax: src_ = 0b000; break;						\
			case ebx: src_ = 0b011; break;						\
			case ecx: src_ = 0b001; break;						\
			case edx: src_ = 0b010; break;						\
			case esi: src_ = 0b110; break;						\
			case edi: src_ = 0b111; break;						\
		}														\
																\
		switch (dst)											\
		{														\
			case rax: dst_ = 0b000; break;						\
			case rbx: dst_ = 0b011; break;						\
			case rcx: dst_ = 0b001; break;						\
			case rdx: dst_ = 0b010; break;						\
			case rsi: dst_ = 0b110; break;						\
			case rdi: dst_ = 0b111; break;						\
		}														\
																\
		EMIT_BYTE(0x89);										\
																\
		/* Craft Mod/RM Byte */									\
    	EMIT_BYTE((0b01 << 6) | (src_ << 3) | (dst_));			\
		EMIT_BYTE(off);											\
	})

#define movb_reg16_indirect_reg64_offset(dst, src, off)			\
	({															\
		uint8_t src_, dst_;										\
																\
		static_assert(dst != ax,								\
			"mov ax, [reg + offset] has a specialized macro!");	\
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

#define movw_reg32_indirect_reg64_offset(dst, src, off)			\
	({															\
		uint8_t src_, dst_;										\
																\
		static_assert(dst != eax,								\
		"mov eax, [reg + offset] has a specialized macro!");	\				
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
			case ebx: dst_ = 0b011; break;						\
			case ecx: dst_ = 0b001; break;						\
			case edx: dst_ = 0b010; break;						\
			case esi: dst_ = 0b110; break;						\
			case edi: dst_ = 0b111; break;						\
		}														\
																\
		EMIT_BYTE(0x8B);										\
																\
		/* Craft Mod/RM Byte */									\
    	EMIT_BYTE((0b01 << 6) | (dst_ << 3) | (src_));			\
		EMIT_BYTE(off);											\
	})

#define movb_ax_indirect_reg64(reg)								\
	({                                  						\
		uint8_t reg_;											\
																\
		switch (reg)											\
		{														\
			case rax: reg_ = 0x00; break;						\
			case rbx: reg_ = 0x03; break;						\
			case rcx: reg_ = 0x01; break;						\
			case rdx: reg_ = 0x02; break;						\
			case rsi: reg_ = 0x06; break;						\
			case rdi: reg_ = 0x07; break;						\
		}														\
																\
		EMIT_BYTE(0x66);										\
		EMIT_BYTE(0x8B);										\
		EMIT_BYTE(reg_);										\
	})
