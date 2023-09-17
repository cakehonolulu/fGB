#include <mmu/mmu.hh>
#include <cstdio>

Mmu :: Mmu(std::vector<char> *bootrom)
{
    mmu = new gb_mmu_t;

    std::memset(mmu, 0, sizeof(gb_mmu_t));

    if (bootrom->size() >= GB_BOOTROM_SIZE)
    {
        memcpy(mmu->gb_bootrom, bootrom->data(), GB_BOOTROM_SIZE);
        std::cout << "Loaded bootrom correctly..." << std::endl;
    }
    else
    {
        std::cout << "Bootrom could not be loaded, exiting..." << std::endl;
        std::exit(1);
    }
    
    mmu->in_bootrom = true;
}

Mmu :: ~Mmu()
{
    delete mmu;
}

void Mmu :: load_game(std::vector<char> *game, std::uint32_t size)
{
    memcpy(mmu->gb_mmap.cart, game->data(), size);
}

void Mmu :: write_byte(std::uint8_t byte, std::uint16_t offset)
{
    // While on BootROM, 0x00 -> 0xFF are R/O
    if (mmu->in_bootrom && offset >= 0x00 && offset <= 0xFF)
    {
        return;
    }

    // #0 Cart (Fixed) [0x0 - 0x3FFF]
    if (offset >= 0x0000 && 0x3FFF >= offset)
    {
        mmu->gb_mmap.cart[offset] = byte;
    }
    else if (offset >= 0x4000 && 0x7FFF >= offset)          // #1 Cart (Switchable) [0x4000 - 0x7FFF]
    {
        mmu->gb_mmap.cart_sw[offset - 0x4000] = byte;
    }
    else if (offset >= 0x8000 && 0x9FFF >= offset)          // VRAM [0x8000 - 0x9FFF]
    {
        mmu->gb_mmap.vram[offset - 0x8000] = byte;
        
        if (offset <= 0x97FF)
        {
            //m_ppu_update_tile(m_dmg, offset);
        }
    }
    else if (offset >= 0xA000 && 0xBFFF >= offset)          // Cartridge RAM (If Exists) [0xA000 - 0xBFFF]
    {
        mmu->gb_mmap.cart_ram[offset - 0xA000] = byte;
    }
    else if (offset >= 0xC000 && 0xDFFF >= offset)          // Working RAM [0xC000 - 0xDFFF]
    {
        mmu->gb_mmap.wram[offset - 0xC000] = byte;
    }
    else if (offset >= 0xE000 && 0xFDFF >= offset)          // Shadow of the Working RAM (Due to PCB layout)
    {
        mmu->gb_mmap.wram[offset - 0xE000] = byte;
    }
    else if (offset >= 0xFE00 && 0xFEFF >= offset)
    {
        mmu->gb_mmap.oam[offset - 0xFE00] = byte;
    }
    else if (offset == 0xFF04)
    {
        // IOREG Handling
    }
    else if (offset == 0xFF05)
    {
        // IOREG Handling
    }
    else if (offset == 0xFF06)
    {
        // IOREG Handling
    }
    else if (offset == 0xFF40)
    {
        std::cout << "Tried to write to LCDC" << std::endl;
        exit(1);
    }
    else if (offset == 0xFF41)
    {
        std::cout << "Tried to write to STAT" << std::endl;
        exit(1);
    }
    else if (offset == 0xFF42)
    {
        std::cout << "Tried to write to Vertical Scroll" << std::endl;
        exit(1);
    }
    else if (offset == 0xFF43)
    {
        std::cout << "Tried to write to Horizontal Scroll" << std::endl;
        exit(1);
    }
    else if(offset == 0xff46)
    {
        // OAM Handling
    }
    else if (offset == 0xFF47)
    {
        //m_ppu_update_palette(m_dmg, byte);
        std::cout << "Tried to write to Palette" << std::endl;
        exit(1);
    }
    else if (offset == 0xFF48)
    {
        // Graphics Handling
    }
    else if (offset == 0xFF49) 
    {
        // Graphics Handling
    }
    else if (offset == 0xFF4A)
    {
        // Graphics Handling
    }
    else if (offset == 0xFF4B)
    {
        // Graphics Handling
    }
    else if (offset == 0xFF00)
    {
        // Key Handling
    }
    else if (offset >= 0xFF00 && 0xFF7F >= offset)
    {
        mmu->gb_mmap.hw_io_reg[offset - 0xFF00] = byte;
    }
    else if (offset >= 0xFF80 && 0xFFFE >= offset) 
    {
        mmu->gb_mmap.zram[offset - 0xFF80] = byte;
    }
    else if (offset == 0xFF0F)
    {
        std::cout << "Tried to write to Interrupt Flag" << std::endl;
        exit(1);
    }
    else if (offset == 0xFFFF)
    {
        std::cout << "Tried to write to Interrupt Enable" << std::endl;
        //exit(1);
    }

    return;
}

std::uint8_t Mmu ::read_byte(uint16_t offset)
{
    if (mmu->in_bootrom && offset >= 0x00 && offset <= 0xFF)
    {
        return mmu->gb_bootrom[offset];
    }

    // #0 Cart (Fixed) [0x0 - 0x3FFF]
    if (offset >= 0x0000 && 0x3FFF >= offset)
    {
        printf("Read from Cart!\n");
        return mmu->gb_mmap.cart[offset];
    }
    else if (offset >= 0x4000 && 0x7FFF >= offset)          // #1 Cart (Switchable) [0x4000 - 0x7FFF]
    {
        return mmu->gb_mmap.cart_sw[offset - 0x4000];
    }
    else if (offset >= 0x8000 && 0x9FFF >= offset)          // VRAM [0x8000 - 0x9FFF]
    {
        return mmu->gb_mmap.vram[offset - 0x8000];
    }
    else if (offset >= 0xA000 && 0xBFFF >= offset)          // Cartridge RAM (If Exists) [0xA000 - 0xBFFF]
    {
        return mmu->gb_mmap.cart_ram[offset - 0xA000];
    }
    else if (offset >= 0xC000 && 0xDFFF >= offset)          // Working RAM [0xC000 - 0xDFFF]
    {
        return mmu->gb_mmap.wram[offset - 0xC000];
    }
    else if (offset >= 0xE000 && 0xFDFF >= offset)          // Shadow of the Working RAM (Due to PCB layout)
    {
        return mmu->gb_mmap.wram[offset - 0xE000];
    }
    else if (offset >= 0xFE00 && 0xFEFF >= offset)
    {
        return mmu->gb_mmap.oam[offset - 0xFE00];
    }
    else if (offset == 0xFF00)
    {
        // GameBoy Key Handling
    }
    else if (offset == 0xFF04)
    {
        // IOREG Handling
    }
    else if (offset == 0xFF05)
    {
        // IOREG Handling
    }
    else if (offset == 0xFF06)
    {
        // IOREG Handling
    }
    else if (offset == 0xFF0F)
    {
        std::cout << "Tried to read from interrupt flags" << std::endl;
        exit(1);
    }
    else if (offset == 0xFF40)
    {
        std::cout << "Tried to read from LCDC" << std::endl;
        exit(1);
    }
    else if (offset == 0xFF41)
    {
        std::cout << "Tried to read from STAT" << std::endl;
        exit(1);
    }
    else if (offset == 0xFF42)
    {
        std::cout << "Tried to read from Vertical Scroll" << std::endl;
        exit(1);
    }
    else if (offset == 0xFF43)
    {
        std::cout << "Tried to read from Horizontal Scroll" << std::endl;
        exit(1);
    }
    else if (offset == 0xFF44)
    {
        std::cout << "Tried to read from Scanlines" << std::endl;
        exit(1);
    }
    else if (offset >= 0xFF00 && 0xFF7F >= offset)
    {
        return mmu->gb_mmap.hw_io_reg[offset - 0xFF00];
    }
    else if (offset >= 0xFF80 && 0xFFFE >= offset) 
    {
        return mmu->gb_mmap.zram[offset - 0xFF80];
    }
    else if (offset == 0xFFFF)
    {
        std::cout << "Tried to read from interrupt enable" << std::endl;
        exit(1);
    }

    return 0;
}

void Mmu :: write_word(std::uint16_t word, std::uint16_t offset)
{
    write_byte(offset, word >> 8);
    write_byte(offset + 1, word & 0xFF);
}

std::uint16_t Mmu :: read_word(std::uint16_t offset)
{
    return ((read_byte(offset) << 8) | (read_byte(offset + 1)));
}