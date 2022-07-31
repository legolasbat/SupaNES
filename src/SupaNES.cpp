#include "SupaNES.h"

SupaNES::SupaNES()
{
    cpu = new CPURicoh();
    ppu = new PPU();
    memory = new Memory(cpu, ppu);
    cart = new Cartridge(memory);
}

void SupaNES::Clock()
{
    int cycles = cpu->Clock();
    ppu->Clock(cycles * 2);
}
