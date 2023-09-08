#pragma once

#include "compiler.h"

#define EMIT(x) (block->data[block->current_len++] = x)
