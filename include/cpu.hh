#pragma once

#include <cstdint>

class Cpu {

private:
	union
	{
		struct {
			std::uint8_t f;
			std::uint8_t a;
		};
		std::uint16_t af;
	};

	union
	{
		struct {
			std::uint8_t c;
			std::uint8_t b;
		};
		std::uint16_t bc;
	};

	union
	{
		struct {
			std::uint8_t l;
			std::uint8_t h;
		};
		std::uint16_t hl;
	};

	std::uint16_t pc;
	std::uint16_t sp;

public:

	Cpu();

	std::uint16_t get_af();
	std::uint8_t get_a();
	void set_af(std::uint16_t af_);
	void set_a(std::uint8_t a_);

	std::uint16_t get_bc();
	std::uint8_t get_b();
	std::uint8_t get_c();
	void set_bc(std::uint16_t bc_);
	void set_b(std::uint8_t b_);
	void set_c(std::uint8_t c_);

	std::uint16_t get_hl();
	std::uint8_t get_h();
	std::uint8_t get_l();
	void set_hl(std::uint16_t hl_);
	void set_h(std::uint8_t h_);
	void set_l(std::uint8_t l_);

	std::uint16_t get_pc();
	void set_pc(std::uint16_t pc_);
	
	std::uint16_t get_sp();
	void set_sp(std::uint16_t sp_);

	std::uint8_t get_f();
	void set_f(std::uint8_t flags_);

	void print_regs();
};
