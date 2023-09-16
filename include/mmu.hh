#pragma once

#include <cstdint>
#include <vector>
#include <cstring>
#include <iostream>

#define GB_BOOTROM_SIZE 256
#define GB_ADDRSPC_SIZE (2^16)

typedef struct gb_mmu
{
	// 256-byte Reserved Frame for Bootrom
	std::uint8_t gb_bootrom[GB_BOOTROM_SIZE];
	std::uint8_t in_bootrom;

	struct
	{
		union
		{
			/*
				Maximum amount of GameBoy's address space comes determined
				by the fact that the GameBoy's Program Counter Register
				is 16-bit only (Which means 2^16 possible memory locations).

				It ranges from $0000 -> $FFFF + 1

				[1] http://gameboy.mongenel.com/dmg/asmmemmap.html

				$0000-$00FF	Restart and Interrupt Vectors
				$0100-$014F	Cartridge Header Area
				$0150-$3FFF	Cartridge ROM - Bank 0 (fixed)
				$4000-$7FFF	Cartridge ROM - Switchable Banks 1-xx
				$8000-$97FF	Character RAM
				$9800-$9BFF	BG Map Data 1
				$9C00-$9FFF	BG Map Data 2
				$A000-$BFFF	Cartridge RAM (If Available)
				$C000-$CFFF	Internal RAM - Bank 0 (fixed)
				$D000-$DFFF	Internal RAM - Bank 1-7 (switchable - CGB only)
				$E000-$FDFF	Echo RAM - Reserved, Do Not Use
				$FE00-$FE9F	OAM - Object Attribute Memory
				$FEA0-$FEFF	Unusable Memory
				$FF00-$FF7F	Hardware I/O Registers
				$FF80-$FFFE	Zero Page - 127 bytes
				$FFFF		Interrupt Enable Flag
			*/
			std::uint8_t gb_address_space[1 << 16];

			struct
			{
				// #1 Cartridge ROM Bank (Fixed)
				union {
					struct {
						// Restart and Interrupt Vectors (0x0 - 0x100)
						std::uint8_t restart_ivt[0x100];
						// ROM Cartridge Header (0x100 - 0x150)
						std::uint8_t cart_hdr[0x50];
						// Cart Size minus IVT and Header
						std::uint8_t cart0[0x3EB0];
					};

					// Cart containing ivt, hdr and cart0
					std::uint8_t cart[0x4000];
				};

				// #2 Cartridge ROM Bank (Switchable)
				std::uint8_t cart_sw[0x4000];

				union {
					struct {
						// Character RAM
						std::uint8_t cram[0x1800];
						// Background Map Data 1
						std::uint8_t bgmap1[0x400];
						// Background Map Data 2
						std::uint8_t bgmap2[0x400];
					};

					// VRAM Containing cram, bgmap[1,2]
					std::uint8_t vram[0x2000];
				};

				// ROM Cartridge RAM (If Exists)
				std::uint8_t cart_ram[0x2000];

				union {
					struct {
						// Bank 0 - Internal RAM
						std::uint8_t int_ram[0x1000];
						// Switchable Bank 1-7 - Internal RAM (CGB Only)
						std::uint8_t int_ram_sw[0x1000];
					};

					// WRAM
					std::uint8_t wram[0x2000];
				};

				// Echo RAM - Reserved
				std::uint8_t echo_ram[0x1E00];

				// Object Attribute Memory
				std::uint8_t oam[0xA0];

				// Unusable Memory
				std::uint8_t unusable[0x60];

				union {
					struct {
						// IO Registers Mapping
						std::uint8_t io[0x40];
						// PPU Mapping
						std::uint8_t ppu[0x40];	
					};

					// Hardware I/O Registers
					std::uint8_t hw_io_reg[0x80];
				};

				union {
					struct {
						// Zero Page - 127 bytes
						std::uint8_t zero_page[0x80 - 1];
						// Interrupt Enable
						std::uint8_t intenable;
					};

					// Zero-Page RAM
					std::uint8_t zram[0x80];
				};

			} gb_mmap;
		};
	};
} gb_mmu_t;

class Mmu {

private:

    gb_mmu_t *mmu;

public:
    Mmu(std::vector<char> *bootrom);
    
    ~Mmu();

	void load_game(std::vector<char> *game, std::uint32_t size);

	void write_byte(std::uint8_t byte, std::uint16_t offset);
	std::uint8_t read_byte(uint16_t offset);

	void write_word(std::uint16_t word, std::uint16_t offset);
	std::uint16_t read_word(std::uint16_t offset);

	static std::uint8_t read_byte_wrapper(Mmu *mmu, std::uint16_t offset) {
		printf("read_byte_wrapper: Read from 0x%04X\n", offset);
		std::uint8_t byte = mmu->read_byte(offset);
		printf("read_byte_wrapper: Value read is 0x%02X\n", byte);
		return byte;
	}

	static void write_byte_wrapper(Mmu *mmu, std::uint8_t byte, std::uint16_t offset) {
		printf("write_byte_wrapper: Write 0x%02X to 0x%04X\n", byte, offset);
		mmu->write_byte(byte, offset);
		printf("write_byte_wrapper: Returned successfully...!\n");
	}

	static std::uint8_t read_word_wrapper(Mmu *mmu, std::uint16_t offset) {
		//printf("write_byte_wrapper: Write 0x%02X to 0x%04X\n", byte, offset);
		return mmu->read_word(offset);
		//printf("write_byte_wrapper: Returned successfully...!\n");
	}

	static void write_word_wrapper(Mmu *mmu, std::uint16_t word, std::uint16_t offset) {
		//printf("write_word_wrapper: Write 0x%04X to 0x%04X\n", word, offset);
		mmu->write_word(word, offset);
		//printf("write_byte_wrapper: Returned successfully...!\n");
	}
};
