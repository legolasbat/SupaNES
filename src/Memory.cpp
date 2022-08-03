#include "Memory.h"
#include <random>

Memory::Memory(CPURicoh* cpu, PPU* ppu, APU* apu) {
	this->cpu = cpu;
	cpu->SetMemory(this);

	this->ppu = ppu;
	ppu->SetMemory(this);

	this->apu = apu;
	apu->SetMemory(this);

	const int rangeFrom = 0;
	const int rangeTo = 0xFF;
	std::random_device randDev;
	std::mt19937 generator(randDev());
	std::uniform_int_distribution<int> distr(rangeFrom, rangeTo);

	for (int i = 0; i < 0x20000; i++)
	{
		//WRAM[i] = distr(generator);
		WRAM[i] = 0;
	}
}

uint8_t Memory::ReadMemory(uint32_t add)
{
	bank = (add & 0x00FF0000) >> 16;
	page = (add & 0x0000FF00) >> 8;
	
	uint8_t value = 0;

	if (bank < 0x40) {
		value = ReadMemoryQ1(add);
	}
	else if (bank < 0x80) {
		value = ReadMemoryQ2(add);
	}
	else if (bank < 0xC0) {
		value = ReadMemoryQ3(add);
	}
	else {
		value = ReadMemoryQ4(add);
	}
	//std::cout << "Reading " << std::hex << (int)add << ": " << std::hex << (int)value << std::endl;
	return value;
}

void Memory::WriteMemory(uint32_t add, uint8_t value)
{
	//std::cout << "Writting " << std::hex << (int)add << ": " << std::hex << (int)value << std::endl;

	bank = (add & 0x00FF0000) >> 16;
	page = (add & 0x0000FF00) >> 8;

	if (add == 0x4210) {
		//std::cout << (int)value << std::endl;
	}

	if (bank < 0x40) {
		WriteMemoryQ1(add, value);
	}
	else if (bank < 0x80) {
		WriteMemoryQ2(add, value);
	}
	else if (bank < 0xC0) {
		WriteMemoryQ3(add, value);
	}
	else {
		WriteMemoryQ4(add, value);
	}
}

void Memory::SetVectors(uint16_t vectors[16])
{
	cpu->SetVectors(vectors);
}

uint8_t Memory::ReadMemoryQ1(uint32_t add)
{
	uint8_t value = 0;

	// WRAM
	if (page < 0x20) {
		value = WRAM[add];
	}
	else if (page == 0x21) {
		// PPU
		if ((add & 0xFF) <= 0x3F) {
			value = ppu->ReadPPU(add);
		}
		// APU
		else if ((add & 0xFF) <= 0x43) {
			value = apu->ReadAPUPort(add);
			//std::cout << "Reading APU: " << std::hex << (int)value << " Q1" << std::endl;
		}
		// WRAM
		else if ((add & 0xFF) == 0x80) {	// Write/Read
			value = WRAM[WRAMAdd];
			WRAMAdd++;
		}
	}
	else if (page == 0x40) {
		std::cout << "Reading Joypad register" << std::endl;
	}
	else if (page == 0x42 || page == 0x43) {
		value = cpu->ReadCPU(add);
	}
	else if (page >= 0x80) {
		value = cart->ReadRom(add);
	}
	return value;
}

uint8_t Memory::ReadMemoryQ2(uint32_t add)
{
	uint8_t value = 0;

	// WRAM
	if (bank == 0x7E || bank == 0x7F) {
		value = WRAM[add - 0x7E0000];
	}
	else {
		value = cart->ReadRom(add);
	}

	return value;
}

uint8_t Memory::ReadMemoryQ3(uint32_t add)
{
	uint8_t value = 0;

	// WRAM
	if (page < 0x20) {
		value = WRAM[0x1FFF & add];
	}
	else if (page == 0x21) {
		// PPU
		if ((add & 0xFF) <= 0x3F) {
			value = ppu->ReadPPU(add);
		}
		// APU
		else if ((add & 0xFF) <= 0x43) {
			value = apu->ReadAPUPort(add);
			//std::cout << "Reading APU: " << std::hex << (int)value << " Q1" << std::endl;
		}
		// WRAM
		else if ((add & 0xFF) == 0x80) {	// Write/Read
			value = WRAM[WRAMAdd];
			WRAMAdd++;
		}
	}
	else if (page == 0x40) {
		std::cout << "Reading Joypad register" << std::endl;
	}
	else if (page == 0x42 || page == 0x43) {
		value = cpu->ReadCPU(add);
	}
	else if (page >= 0x80) {
		value = cart->ReadRom(add);
	}

	return value;
}

uint8_t Memory::ReadMemoryQ4(uint32_t add)
{
	uint8_t value = 0;

	value = cart->ReadRom(add);

	return value;
}

void Memory::WriteMemoryQ1(uint32_t add, uint8_t value)
{
	// WRAM
	if (page < 0x20) {
		WRAM[add] = value;
	}
	else if (page == 0x21) {
		// PPU
		if ((add & 0xFF) <= 0x3F) {
			ppu->WritePPU(add, value);
		}
		// APU
		else if ((add & 0xFF) <= 0x43) {
			apu->WriteCPUPort(add, value);
			//std::cout << "Writting APU in " << std::hex << (int)add << ": " << (int)value << " Q1" << std::endl;
		}
		// WRAM
		else if ((add & 0xFF) == 0x80) {	// Write/Read
			WRAM[WRAMAdd] = value;
			WRAMAdd++;
		}
		else if ((add & 0xFF) == 0x81) {	// Low Address
			WRAMAdd &= 0x1FF00;
			WRAMAdd |= value;
		}
		else if ((add & 0xFF) == 0x82) {	// Page Address
			WRAMAdd &= 0x100FF;
			WRAMAdd |= value << 8;
		}
		else if ((add & 0xFF) == 0x83) {	// Bank Address
			WRAMAdd &= 0xFFFF;
			WRAMAdd |= value << 16;
		}
	}
	else if (page == 0x40) {
		//std::cout << "Writting Joypad register" << std::endl;
	}
	else if (page == 0x42 || page == 0x43) {
		cpu->WriteCPU(add, value);
	}
}

void Memory::WriteMemoryQ2(uint32_t add, uint8_t value)
{
	// WRAM
	if (bank == 0x7E || bank == 0x7F) {
		WRAM[add - 0x7E0000] = value;
	}
	else {
		// SRAM
		if (page < 0x80) {
			cart->WriteRom(add, value);
		}
	}
}

void Memory::WriteMemoryQ3(uint32_t add, uint8_t value)
{
	// WRAM
	if (page < 0x20) {
		WRAM[0x1FFF & add] = value;
	}
	else if (page == 0x21) {
		// PPU
		if ((add & 0xFF) <= 0x3F) {
			ppu->WritePPU(add, value);
		}
		// APU
		else if ((add & 0xFF) <= 0x43) {
			apu->WriteCPUPort(add, value);
			//std::cout << "Writting APU in " << std::hex << (int)add << ": " << (int)value << " Q1" << std::endl;
		}
		// WRAM
		else if ((add & 0xFF) == 0x80) {	// Write/Read
			WRAM[WRAMAdd] = value;
			WRAMAdd++;
		}
		else if ((add & 0xFF) == 0x81) {	// Low Address
			WRAMAdd &= 0x1FF00;
			WRAMAdd |= value;
		}
		else if ((add & 0xFF) == 0x82) {	// Page Address
			WRAMAdd &= 0x100FF;
			WRAMAdd |= value << 8;
		}
		else if ((add & 0xFF) == 0x83) {	// Bank Address
			WRAMAdd &= 0xFFFF;
			WRAMAdd |= value << 16;
		}
	}
	else if (page == 0x40) {
		//std::cout << "Writting Joypad register" << std::endl;
	}
	else if (page == 0x42 || page == 0x43) {
		cpu->WriteCPU(add, value);
	}
}

void Memory::WriteMemoryQ4(uint32_t add, uint8_t value)
{
	// SRAM
	if (bank >= 0xF0 && page < 0x80) {
		cart->WriteRom(add, value);
	}
}
