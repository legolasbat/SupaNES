#pragma once

#include <cstdint>

class Memory;

class PPU
{
private:

	Memory* mem = nullptr;

	int HCounter = 46;
	int VCounter = 0;
	int frameCounter = 0;
	bool frameCompleted = false;

	uint16_t VRAM[0x8000];
	uint16_t VRAMAdd = 0;

	uint16_t CGRAM[0x100];
	uint8_t CGRAMAdd = 0;
	bool CGAddSet = false;

	uint16_t OAM[0x110];
	uint16_t OAMAdd = 0;
	bool OAMAddSet = false;

	uint8_t BGTileAdd1 = 0;
	uint8_t BGTileAdd2 = 0;
	uint8_t BGTileAdd3 = 0;
	uint8_t BGTileAdd4 = 0;
	uint8_t BGAdd1_2 = 0;
	uint8_t BGAdd3_4 = 0;

public:

	void Clock(int cycles);

	void SetMemory(Memory* mem) {
		this->mem = mem;
	}

	bool IsFrameCompleted() {
		return frameCompleted;
	}

	int GetFrame() {
		frameCompleted = false;
		return 0;
	}

	uint8_t ReadPPU(uint32_t add);
	void WritePPU(uint32_t add, uint8_t value);

	int GetVCounter();
	int GetHCounter();
	int GetFrameCounter();
};

