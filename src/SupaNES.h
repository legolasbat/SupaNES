#pragma once

#include "Memory.h"

class SupaNES
{
private:
	Memory* memory;
	APU* apu;

	int64_t scheduler_CPU_SPC700 = 0;
	const uint32_t CPU_MASTER_CLOCK = 21477272; // PAL
	const uint32_t SPC700_MASTER_CLOCK = 24576000;

public:
	Cartridge* cart;
	CPURicoh* cpu;
	PPU* ppu;

	SupaNES();

	int Clock();

};

