#pragma once
#include "SDL_syswm.h"

#include <commdlg.h>
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>

class Memory;

class Cartridge
{

private:
	enum class ROM_chipset {
		ROM_only = 0b0000,
		ROM_SRAM = 0b0001,
		ROM_SRAM_BATTERY = 0b0010,
		ROM_CO_CPU = 0b0011,
		ROM_CO_CPU_SRAM = 0b0100,
		ROM_CO_CPU_SRAM_BATTERY = 0b0101,
		ROM_CO_CPU_BATTERY = 0b0110
	};

	std::map<uint8_t, std::string> ROM_chipset_string = {
		{0b0000, "ROM only"},
		{0b0001, "ROM + SRAM"},
		{0b0010, "ROM + SRAM & Battery"},
		{0b0011, "ROM + Co-CPU"},
		{0b0100, "ROM + Co-CPU + SRAM"},
		{0b0101, "ROM + Co-CPU + SRAM & Battery"},
		{0b0110, "ROM + Co-CPU + Battery"}
	};

	enum class ROM_coprocessor {
		DSP = 0b0000,
		SuperFX = 0b0001,
		OBC1 = 0b0010,
		SA_1 = 0b0011,
		S_DD1 = 0b0100,
		S_RTC = 0b0101,
		Other = 0b1000,
		Custom = 0b1111
	};

	std::map<uint8_t, std::string> ROM_coprocessor_string = {
		{0b0000, "DSP"},
		{0b0001, "SuperFX"},
		{0b0010, "OBC1"},
		{0b0011, "SA-1"},
		{0b0100, "S-DD1"},
		{0b0101, "S-RTC"},
		{0b1000, "Other"},
		{0b1111, "Custom (v2/v3 header)"}
	};

	std::map<uint8_t, std::string> Region = {
		{0x00, "Japan"},
		{0x01, "USA"},
		{0x02, "Europe"},
		{0x03, "Sweden"},
		{0x04, "Japan"},
		{0x05, "Denmark"},
		{0x06, "France"},
		{0x07, "Netherlands"},
		{0x08, "Spain"},
		{0x09, "Germany"},
		{0x0a, "Italy"},
		{0x0b, "China"},
		{0x0c, "Indonesia"},
		{0x0d, "South Korea"},
		{0x0e, "Common/International"},
		{0x0f, "Canada"},
		{0x10, "Brazil"},
		{0x11, "Australia"}
	};

	Memory* mem;

	std::vector<uint8_t> ROM;

public:
	Cartridge(Memory* mem) {
		this->mem = mem;
	}

	bool LoadRom(OPENFILENAME gameDir);

	uint8_t ReadRom(uint32_t add);
	void WriteRom(uint32_t add, uint8_t value);
};

