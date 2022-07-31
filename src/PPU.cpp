#include "PPU.h"

#include "Memory.h"

void PPU::Clock(int cycles)
{
	HCounter += cycles;
	if (HCounter >= 339) {
		HCounter -= 339;
		VCounter++;
		if (VCounter == 225) {
			mem->cpu->NMI();
			frameCompleted = true;
		}
		if (VCounter == 262) {
			frameCounter++;
			if (frameCounter >= 60) {
				frameCounter = 0;
			}
			VCounter = 0;
		}
	}
}

uint8_t PPU::ReadPPU(uint32_t add)
{
	uint8_t bank = (add & 0x00FF0000) >> 16;
	uint8_t page = (add & 0x0000FF00) >> 8;
	uint8_t reg = add & 0xFF;

	uint8_t value = 0;

	if (page == 0x21) {

		// VRAM
		if (reg == 0x39) {	// Low Byte Read
			value = VRAM[VRAMAdd] & 0xFF;
		}
		else if (reg == 0x3A) {	// High Byte Read
			value = VRAM[VRAMAdd] >> 8;
			VRAMAdd++;
		}
		// OAM
		else if (reg == 0x38) {	// Read Twice L/H
			if (OAMAddSet) {
				value = OAM[OAMAdd] & 0xFF;
				OAMAddSet = false;
			}
			else {
				value = OAM[OAMAdd] >> 8;
				OAMAdd++;
				OAMAddSet = true;
			}
		}
		// CGRAM
		else if (reg == 0x3B) {	// Read Twice L/H
			if (CGAddSet) {
				value = CGRAM[CGRAMAdd] & 0xFF;
				CGAddSet = false;
			}
			else {
				value = CGRAM[CGRAMAdd] >> 8;
				CGRAMAdd++;
				CGAddSet = true;
			}
		}
		// WRAM
		else if (reg == 0x80) {	// Write/Read

		}
		else if (reg == 0x81) {	// Low Address

		}
		else if (reg == 0x82) {	// Page Address

		}
		else if (reg == 0x83) {	// Bank Address

		}
	}

	return value;
}

void PPU::WritePPU(uint32_t add, uint8_t value)
{
	uint8_t bank = (add & 0x00FF0000) >> 16;
	uint8_t page = (add & 0x0000FF00) >> 8;
	uint8_t reg = add & 0xFF;

	if (page == 0x21) {
		if (reg == 0x07) {
			BGTileAdd1 = value;
		}
		else if (reg == 0x08) {
			BGTileAdd2 = value;
		}
		else if (reg == 0x09) {
			BGTileAdd3 = value;
		}
		else if (reg == 0x0A) {
			BGTileAdd4 = value;
		}
		else if (reg == 0x0B) {
			BGAdd1_2 = value;
		}
		else if (reg == 0x0C) {
			BGAdd3_4 = value;
		}
		// VRAM
		else if (reg == 0x16) {	// Low Byte Address
			VRAMAdd &= 0xFF00;
			VRAMAdd |= value;
		}
		else if (reg == 0x17) {	// High Byte Address
			VRAMAdd &= 0x00FF;
			VRAMAdd |= value << 8;
		}
		else if (reg == 0x18) {	// Low Byte Write
			VRAM[VRAMAdd] = value;
		}
		else if (reg == 0x19) {	// High Byte Write
			VRAM[VRAMAdd] |= value << 8;
			VRAMAdd++;
		}
		// OAM
		else if (reg == 0x02) {	// Low Byte Address
			OAMAdd &= 0xFF00;
			OAMAdd |= value;
		}
		else if (reg == 0x03) {	// High Byte Address
			OAMAdd &= 0x00FF;
			OAMAdd |= value << 8;
			OAMAddSet = true;
		}
		else if (reg == 0x04) {	// Write Twice L/H
			if (OAMAddSet) {
				OAM[OAMAdd] = value;
				OAMAddSet = false;
			}
			else {
				OAM[OAMAdd] |= value << 8;
				OAMAdd++;
				OAMAddSet = true;
			}
		}
		// CGRAM
		else if (reg == 0x21) {	// Address
			CGRAMAdd = value;
			CGAddSet = true;
		}
		else if (reg == 0x22) {	// Write Twice L/H
			if (CGAddSet) {
				CGRAM[CGRAMAdd] = value;
				CGAddSet = false;
			}
			else {
				CGRAM[CGRAMAdd] |= value << 8;
				CGRAMAdd++;
				CGAddSet = true;
			}
		}
		// WRAM
		else if (reg == 0x80) {	// Write/Read

		}
		else if (reg == 0x81) {	// Low Address

		}
		else if (reg == 0x82) {	// Page Address

		}
		else if (reg == 0x83) {	// Bank Address

		}
	}

}

int PPU::GetVCounter() {
	return VCounter;
}

int PPU::GetHCounter() {
	return HCounter;
}

int PPU::GetFrameCounter() {
	return frameCounter;
}
