#include <jit/generic_block.hh>
#include <capstone/capstone.h>
#include <fgb.hh>
#include <iostream>

#if __has_include(<format>)
    #include <format>
    using std::format;
#else
    #include <fmt/format.h>
    using fmt::format;
#endif

Block :: Block()
{
	pc = 0xFFFF;
	id = (uint16_t) rand();
	dirty = false;
}

Block :: Block(uint8_t id_, uint16_t pc_)
	: pc(pc_), id(id_)
{
}

Block :: ~Block() {
}

bool Block :: is_dirty() {
	return dirty;
}

void Block :: set_dirty(bool dirty_) {
	dirty = dirty_;
}

std::uint16_t Block :: get_pc() {
	return pc;
}

void Block :: set_pc(std::uint16_t pc_) {
	pc = pc_;
}

std::uint16_t Block :: get_id() {
	return id;
}

void Block :: set_id(std::uint16_t id_) {
	id = id_;
}
