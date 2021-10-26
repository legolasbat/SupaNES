#include "Memory.h"

Memory::Memory(CPURicoh* cpu) {
	this->cpu = cpu;
	cpu->SetMemory(this);
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
	
	return value;
}

void Memory::WriteMemory(uint32_t add, uint8_t value)
{
	bank = (add & 0x00FF0000) >> 16;
	page = (add & 0x0000FF00) >> 8;

	if (add == 0x4210) {
		std::cout << (int)value << std::endl;
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
		std::cout << "Reading PPU register" << std::endl;
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
		std::cout << "Reading PPU register" << std::endl;
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
		std::cout << "Writting PPU register" << std::endl;
	}
	else if (page == 0x40) {
		std::cout << "Writting Joypad register" << std::endl;
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
}

void Memory::WriteMemoryQ3(uint32_t add, uint8_t value)
{
	// WRAM
	if (page < 0x20) {
		WRAM[0x1FFF & add] = value;
	}
	else if (page == 0x21) {
		std::cout << "Writting PPU register" << std::endl;
	}
	else if (page == 0x40) {
		std::cout << "Writting Joypad register" << std::endl;
	}
	else if (page == 0x42 || page == 0x43) {
		cpu->WriteCPU(add, value);
	}
}

void Memory::WriteMemoryQ4(uint32_t add, uint8_t value)
{
}
