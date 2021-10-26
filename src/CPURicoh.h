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

	uint8_t NMI = 0xc2;

	bool emulationMode;

	bool movActive = false;
	uint8_t srcbk = 0;

	uint8_t opcode = 0;
	int cycles = 0;

	// Native mode vectors
	uint16_t COPVector;		// 0x00FFE4
	uint16_t BRKVector;		// 0x00FFE6
	uint16_t NMIVector;		// 0x00FFEA
	uint16_t IRQVector;		// 0x00FFEE

	// Emulation mode vectors
	uint16_t ResetVector;	// 0x00FFFC

	Memory* mem;

	uint8_t ReadMemory(uint32_t add, bool isLong);
	void WriteMemory(uint32_t add, uint8_t value, bool isLong);

	uint8_t Pull();
	void Push(uint8_t value);

	int Execute();

	void CheckNFlag(uint16_t value, bool isA, bool isX);
	void CheckZFlag(uint16_t value, bool isA, bool isX);
	void CheckCFlag(uint16_t value, uint16_t prevValue, bool isSub);

	int extraCycles = 0;

	void ADC(uint16_t value);
	void AND(uint16_t value);
	void CMP(uint16_t value);
	void CPX(uint16_t value);
	void CPY(uint16_t value);
	void EOR(uint16_t value);
	void ORA(uint16_t value);
	void SBC(uint16_t value);

	enum class AddMode {
		Immediate,
		Relative,
		RelativeLong,
		Direct,
		DirectIndexedX,
		DirectIndexedY,
		DirectIndirect,
		DirectIndexedIndirect,
		DirectIndirectIndexed,
		DirectIndirectLong,
		DirectIndirectIndexedLong,
		Absolute,
		AbsoluteIndexedX,
		AbsoluteIndexedY,
		AbsoluteLong,
		AbsoluteIndexedLong,
		StackRelative,
		StackRelativeIndirectIndexed,
		AbsoluteIndirect,
		AbsoluteIndirectLong,
		AbsoluteIndexedIndirect
	};

	uint16_t GetValue(AddMode addMode, bool is16);

public:

	CPURicoh();

	int Clock();

	void Debug();

	void SetMemory(Memory* mem) {
		this->mem = mem;
	}

	void SetVectors(uint16_t vectors[16]);

	uint8_t ReadCPU(uint32_t add);
	void WriteCPU(uint32_t add, uint8_t value);
};

