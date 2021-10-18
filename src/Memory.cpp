#include "Memory.h"

void Memory::InitLoRom()
{
	std::cout << "Initializing memory of type LoROM..." << std::endl;
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

uint8_t Memory::ReadMemoryQ1(uint32_t add)
{
	uint8_t value = 0;

	// WRAM
	if (page < 0x20) {
		value = WRAM[add & 0x1FFF];
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

	return value;
}

uint8_t Memory::ReadMemoryQ3(uint32_t add)
{
	uint8_t value = 0;

	// WRAM
	if (page < 0x20) {
		value = WRAM[add & 0x1FFF];
	}

	return value;
}

uint8_t Memory::ReadMemoryQ4(uint32_t add)
{
	return uint8_t();
}

void Memory::WriteMemoryQ1(uint32_t add, uint8_t value)
{
	// WRAM
	if (page < 0x20) {
		WRAM[add & 0x1FFF] = value;
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
		WRAM[add & 0x1FFF] = value;
	}
}

void Memory::WriteMemoryQ4(uint32_t add, uint8_t value)
{
}
