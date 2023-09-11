#include <cpu.hh>
#include <iostream>

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


std::uint8_t Cpu :: get_flags() {
    return flags;
}

void Cpu :: set_flags(std::uint8_t flags_) {
    flags = flags_;
}

void Cpu :: print_regs() {
    std::cout << "A:  " << format("{:#04x}", get_a()) << ", F: " << format("{:#04x}", get_f()) << ", AF: " << format("{:#06x}", get_af()) << "\n";
    std::cout << "B:  " << format("{:#04x}", get_b()) << ", C: " << format("{:#04x}", get_c()) << ", BC: " << format("{:#06x}", get_bc()) << "\n";
    std::cout << "H:  " << format("{:#04x}", get_h()) << ", L: " << format("{:#04x}", get_l()) << ", HL: " << format("{:#06x}", get_hl()) << "\n";
    std::cout << "PC: " << format("{:#06x}", get_pc()) << "         SP: " << format("{:#06x}", get_sp()) << "\n";
}
