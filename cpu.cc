#include <cpu.hh>
#include <fgb.hh>
#include <iostream>
#include <bitset>

#if __has_include(<format>)
    #include <format>
    using std::format;
#else
    #include <fmt/format.h>
    using fmt::format;
#endif

Cpu :: Cpu()
{
    af = 0;
    bc = 0;
    hl = 0;
    pc = 0;
    sp = 0;
}


std::uint16_t Cpu :: get_af() {
    return af;
}

std::uint8_t Cpu :: get_a() {
    return a;
}

std::uint8_t Cpu :: get_f() {
    return f;
}

void Cpu :: set_af(std::uint16_t af_) {
    af = af_;
}

void Cpu :: set_a(std::uint8_t a_) {
    a = a_;
}

void Cpu :: set_f(std::uint8_t f_) {
    f = f_;
}


std::uint16_t Cpu :: get_bc() {
    return bc;
}

std::uint8_t Cpu :: get_b() {
    return b;
}

std::uint8_t Cpu :: get_c() {
    return c;
}

void Cpu :: set_bc(std::uint16_t bc_) {
    bc = bc_;
}

void Cpu :: set_b(std::uint8_t b_) {
    b = b_;
}

void Cpu :: set_c(std::uint8_t c_) {
    c = c_;
}

std::uint16_t Cpu :: get_de() {
    return de;
}

std::uint8_t Cpu :: get_d() {
    return d;
}

std::uint8_t Cpu :: get_e() {
    return e;
}

void Cpu :: set_de(std::uint16_t de_) {
    de = de_;
}

void Cpu :: set_d(std::uint8_t d_) {
    d = d_;
}

void Cpu :: set_e(std::uint8_t e_) {
    e = e_;
}

std::uint16_t Cpu :: get_hl() {
    return hl;
}

std::uint8_t Cpu :: get_h() {
    return h;
}

std::uint8_t Cpu :: get_l() {
    return l;
}

void Cpu :: set_hl(std::uint16_t hl_) {
    hl = hl_;
}

void Cpu :: set_h(std::uint8_t h_) {
    h = h_;
}

void Cpu :: set_l(std::uint8_t l_) {
    l = l_;
}

std::uint16_t Cpu :: get_pc() {
    return pc;
}

void Cpu :: set_pc(std::uint16_t pc_) {
    pc = pc_;
}

	
std::uint16_t Cpu :: get_sp() {
    return sp;
}

void Cpu :: set_sp(std::uint16_t sp_) {
    sp = sp_;
}

void Cpu :: print_regs() {
    std::bitset<4> flags(((std::bitset<8>) (get_f() & 0xF0)).to_string().substr(0, 4));

    std::cout << "A:  0x" << format("{:02X}", get_a()) << ", F: 0x" << format("{:02X}", get_f()) << ", AF: 0x" << format("{:04X}", get_af()) << "\n";
    std::cout << "B:  0x" << format("{:02X}", get_b()) << ", C: 0x" << format("{:02X}", get_c()) << ", BC: 0x" << format("{:04X}", get_bc()) << "\n";
    std::cout << "D:  0x" << format("{:02X}", get_d()) << ", E: 0x" << format("{:02X}", get_e()) << ", DE: 0x" << format("{:04X}", get_de()) << "\n";
    std::cout << "H:  0x" << format("{:02X}", get_h()) << ", L: 0x" << format("{:02X}", get_l()) << ", HL: 0x" << format("{:04X}", get_hl()) << "\n";
    std::cout << "PC: 0x" << format("{:04X}", get_pc()) << "         SP: 0x" << format("{:04X}", get_sp()) << "\n";
    std::cout << "Flags: 0x" << format("{:02X}", (get_f())) << " (0b" << flags.to_string() << "), ";

    if (flags.to_string().at(0) == '1')
    {
        std::cout << BOLDGREEN << "Z ";
    }
    else
    {
        std::cout << BOLDRED << "Z ";
    }
    
    if (flags.to_string().at(1) == '1')
    {
        std::cout << BOLDGREEN << "N ";
    }
    else
    {
        std::cout << BOLDRED << "N ";
    }
    
    if (flags.to_string().at(2) == '1')
    {
        std::cout << BOLDGREEN << "H ";
    }
    else
    {
        std::cout << BOLDRED << "H ";
    }
    
    if (flags.to_string().at(3) == '1')
    {
        std::cout << BOLDGREEN << "C " << RESET << std::endl;
    }
    else
    {
        std::cout << BOLDRED << "C " << RESET << std::endl;
    }
}
