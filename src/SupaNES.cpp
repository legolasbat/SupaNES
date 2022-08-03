#include "SupaNES.h"

SupaNES::SupaNES()
{
    cpu = new CPURicoh();
    ppu = new PPU();
    apu = new APU();
    memory = new Memory(cpu, ppu, apu);
    cart = new Cartridge(memory);
}

int SupaNES::Clock()
{
    int cycles = 0;
    ////CPU
    if (scheduler_CPU_SPC700 >= 0) {
        cycles = cpu->Clock();
        uint16_t cpu_master_cycles = cycles * 6; // because PAL cpu clock = 21.2813700MHz / 6
        scheduler_CPU_SPC700 -= cpu_master_cycles * SPC700_MASTER_CLOCK;
    
        ppu->Clock(cycles * 2);
    }
    //SPC700
    else {
        uint16_t spc700_cycles = apu->Clock();
        uint16_t spc700_master_cycles = spc700_cycles * 24; // because SPC700 cpu clock = 24.576MHz/24
        scheduler_CPU_SPC700 += spc700_master_cycles * CPU_MASTER_CLOCK;
    }

    //int cycles = cpu->Clock();
    //ppu->Clock(cycles * 2);
    //int spc700_cycles = apu->Clock();

    return cycles;
}
