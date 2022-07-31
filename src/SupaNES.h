#pragma once

#include "Cartridge.h"
#include "Memory.h"
#include "PPU.h"

class SupaNES
{
private:
	Memory* memory;
	CPURicoh* cpu;

public:
	Cartridge* cart;
	PPU* ppu;

	SupaNES();

	void Clock();

};

