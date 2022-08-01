#pragma once

#include "Cartridge.h"
#include "CPURicoh.h"
#include "PPU.h"
#include "APU.h"

class Memory
{
public:

	Memory(CPURicoh* cpu, PPU* ppu, APU* apu);

	void SetRom(Cartridge* cart) {
		this->cart = cart;
	}

	uint8_t ReadMemory(uint32_t add);
	void WriteMemory(uint32_t add, uint8_t value);
	void SetVectors(uint16_t vectors[16]);

	CPURicoh* cpu;
	PPU* ppu;
	APU* apu;
private:
	
	uint8_t WRAM[0x20000];

	Cartridge* cart;

	uint8_t bank = 0;
	uint8_t page = 0;

	uint8_t ReadMemoryQ1(uint32_t add);
	uint8_t ReadMemoryQ2(uint32_t add);
	uint8_t ReadMemoryQ3(uint32_t add);
	uint8_t ReadMemoryQ4(uint32_t add);

	void WriteMemoryQ1(uint32_t add, uint8_t value);
	void WriteMemoryQ2(uint32_t add, uint8_t value);
	void WriteMemoryQ3(uint32_t add, uint8_t value);
	void WriteMemoryQ4(uint32_t add, uint8_t value);

};

