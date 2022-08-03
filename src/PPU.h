#pragma once

#include <cstdint>
#include <iomanip>

class Memory;

class PPU
{
private:

	Memory* mem = nullptr;

	int channels = 1;
	int width = 256;
	int height = 224;

	uint16_t* pixels = new uint16_t[width * height * channels];	// width * height * color channels

	int HCounter = 46;
	int VCounter = 0;
	int frameCounter = 0;
	bool frameCompleted = false;

	uint8_t VMain = 0;
	uint16_t VRAM[0x8000];
	uint16_t VRAMAdd = 0;

	uint16_t CGRAM[0x100];
	uint8_t CGRAMAdd = 0;
	bool CGAddSet = false;

	uint16_t OAM[0x110];
	uint16_t OAMAdd = 0;
	bool OAMAddSet = false;

	// 0x2105
	uint8_t BGMode = 0;

	enum class BPP {
		BPP2,
		BPP4,
		BPP8
	};

	// 0x212C
	uint8_t TM = 0;

	uint8_t mosaicReg = 0;

	uint8_t BGTileAdd1 = 0;	// 0x2107
	uint8_t BGTileAdd2 = 0;	// 0x2108
	uint8_t BGTileAdd3 = 0;	// 0x2109
	uint8_t BGTileAdd4 = 0;	// 0x210A
	uint8_t BGAdd1_2 = 0;	// 0x210B
	uint8_t BGAdd3_4 = 0;	// 0x210C
	bool BG1XScrollWritten = false;
	uint16_t BG1XScroll = 0;// 0x210D
	bool BG1YScrollWritten = false;
	uint16_t BG1YScroll = 0;// 0x210E
	bool BG2XScrollWritten = false;
	uint16_t BG2XScroll = 0;// 0x210F
	bool BG2YScrollWritten = false;
	uint16_t BG2YScroll = 0;// 0x2110
	bool BG3XScrollWritten = false;
	uint16_t BG3XScroll = 0;// 0x2111
	bool BG3YScrollWritten = false;
	uint16_t BG3YScroll = 0;// 0x2112
	bool BG4XScrollWritten = false;
	uint16_t BG4XScroll = 0;// 0x2113
	bool BG4YScrollWritten = false;
	uint16_t BG4YScroll = 0;// 0x2114


	void SetPixel(int X, int Y);
	void SetBGPixel(int X, int Y, uint8_t BGAdd, uint8_t BGIndex, BPP bpp, uint8_t XSize, uint8_t YSize, uint16_t XScroll, uint16_t YScroll);
	void SetBGDrop(int X, int Y);
	BPP GetColorBPPForBG(int BGIndex);
	uint16_t GetColor(uint8_t id, uint8_t tilePal);
	uint16_t GetColor2BPP(uint16_t charAdd, uint8_t pal, uint8_t HShift, int BGNumber);
	uint16_t GetColor4BPP(uint16_t charAdd, uint8_t pal, uint8_t HShift);
	uint16_t GetColor8BPP(uint16_t charAdd, uint8_t pal, uint8_t HShift);

	uint8_t GetMode();
	bool GetSizeBG(uint8_t BGNumber);

	// Debug
	bool debug = false;
	bool started = false;
	FILE* LOG;

	bool BG1Active = true;
	bool BG2Active = true;
	bool BG3Active = true;
	bool BG4Active = true;


public:

	void Clock(int cycles);

	void SetMemory(Memory* mem) {
		this->mem = mem;
	}

	bool IsFrameCompleted() {
		return frameCompleted;
	}

	uint16_t* GetFrame();

	uint8_t ReadPPU(uint32_t add);
	void WritePPU(uint32_t add, uint8_t value);

	int GetVCounter();
	int GetHCounter();
	int GetFrameCounter();
	int GetPitch() {
		return width * sizeof(uint16_t) * channels;
	}

	// Debug
	void ToggleBG1(bool state);
	void ToggleBG2(bool state);
	void ToggleBG3(bool state);
	void ToggleBG4(bool state);

};

