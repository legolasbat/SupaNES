#pragma once

#include <cstdint>

// Flags stored in PSW Register
#define NFlag 0x80	// Negative
#define VFlag 0x40	// Overflow
#define PFlag 0x20	// Direct page
#define BFlag 0x10	// Break
#define HFlag 0x08	// Half carry
#define IFlag 0x04	// Interrupt enabled (unused)
#define ZFlag 0x02	// Zero
#define CFlag 0x01	// Carry

class Memory;

class APU
{
private:

	// Internal registers

	// Program Counter
	uint16_t PC;

	// Accumulator
	uint8_t A;

	// Index
	uint8_t X, Y;

	// Stack Pointer
	uint8_t SP;

	// Program Status Word
	uint8_t PSW;

	uint8_t opcode = 0;
	int cycles = 0;

	Memory* mem;

	uint8_t page0[0xF0];

	uint8_t registers0[0x4];
	uint8_t CPUPorts[0x4];	// CPU Write ports & APU Read ports
	uint8_t APUPorts[0x4];	// APU Write ports & CPU Read ports
	uint8_t registers1[0x8];

	uint8_t page1[0x100];

	uint8_t ARAM[0xFE00];

	void Debug();

	uint8_t ReadAPUMem(uint16_t add);
	void WriteAPUMem(uint16_t add, uint8_t value);

	uint8_t Pull();
	void Push(uint8_t value);

	uint8_t ReadCPUPort(uint8_t add);
	void WriteAPUPort(uint8_t add, uint8_t value);

	void CheckNFlag(uint8_t value);
	void CheckZFlag(uint8_t value);
	void CheckCFlag(uint8_t value, uint8_t prevValue, bool isSub);

	void ADC(uint8_t value);
	void SBC(uint8_t value);
	void CMP(uint8_t valueA, uint8_t valueB);

	uint8_t bootRom[64] = {
	0xCD, 0xEF, 0xBD, 0xE8, 0x00, 0xC6, 0x1D, 0xD0,
	0xFC, 0x8F, 0xAA, 0xF4, 0x8F, 0xBB, 0xF5, 0x78,
	0xCC, 0xF4, 0xD0, 0xFB, 0x2F, 0x19, 0xEB, 0xF4,
	0xD0, 0xFC, 0x7E, 0xF4, 0xD0, 0x0B, 0xE4, 0xF5,
	0xCB, 0xF4, 0xD7, 0x00, 0xFC, 0xD0, 0xF3, 0xAB,
	0x01, 0x10, 0xEF, 0x7E, 0xF4, 0x10, 0xEB, 0xBA,
	0xF6, 0xDA, 0x00, 0xBA, 0xF4, 0xC4, 0xF4, 0xDD,
	0x5D, 0xD0, 0xDB, 0x1F, 0x00, 0x00, 0xC0, 0xFF
	};

public:

	APU();

	int Clock();

	int Execute();

	void SetMemory(Memory* mem) {
		this->mem = mem;
	}

	void WriteCPUPort(uint8_t add, uint8_t value);
	uint8_t ReadAPUPort(uint8_t add);
};

