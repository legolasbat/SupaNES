#pragma once

#include <cstdint>
#include <iomanip>

#define NFlag 0x80
#define VFlag 0x40
#define MFlag 0x20
#define XFlag 0x10	// Break flag in emulation mode only
#define DFlag 0x08	// Unused
#define IFlag 0x04
#define ZFlag 0x02
#define CFlag 0x01	// Also the emulation flag

class Memory;

class CPURicoh
{
private:
	
	// Internal registers

	// Program Counter
	uint16_t PC;

	// Accumulator
	uint16_t A;

	//Index
	uint16_t X, Y;

	// Stack Pointer
	uint16_t SP;

	// Direct Page
	uint16_t DP;

	// Program Bank
	uint8_t PB;

	// Data Bank
	uint8_t DB;

	// Processor Status
	uint8_t P;

	bool emulationMode;

	uint8_t opcode = 0;
	int cycles = 0;

	Memory* mem;

	uint8_t ReadMemory(uint32_t add);
	void WriteMemory(uint32_t add, uint8_t value);

	int Execute();

	void CheckNFlag(uint16_t value);
	void CheckZFlag(uint16_t value);

public:

	CPURicoh();

	int Clock();

	void Debug();

	void SetMemory(Memory* mem) {
		this->mem = mem;
	}

};

