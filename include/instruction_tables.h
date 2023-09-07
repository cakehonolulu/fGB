#pragma once

#include <instructions.h>
#include <extended_instructions.h>
#include <stddef.h>

extern struct instructions {
	void *function;
} const instrs[256];

extern struct extended_instructions {
	void *function;
} const extended_instrs[256];
