#include "PPU.h"

#include "Memory.h"

void PPU::Clock(int cycles)
{
	for (int i = 0; i < cycles; i++)
	{
		if (HCounter < width && VCounter < height) {
			SetPixel(HCounter, VCounter);
		}
		HCounter++;

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
			if ((VMain & 0x80) == 0) {
				VRAMAdd++;
			}
		}
		else if (reg == 0x3A) {	// High Byte Read
			value = VRAM[VRAMAdd] >> 8;
			if ((VMain & 0x80) == 0x80) {
				VRAMAdd++;
			}
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
	}

	return value;
}

void PPU::WritePPU(uint32_t add, uint8_t value)
{
	uint8_t bank = (add & 0x00FF0000) >> 16;
	uint8_t page = (add & 0x0000FF00) >> 8;
	uint8_t reg = add & 0xFF;

	if (page == 0x21) {
		if (reg == 0x05) {
			BGMode = value;
			std::cout << std::hex << "Mode: " << (int)BGMode << std::endl;
		}
		else if (reg == 0x06) {
			mosaicReg = value;
		}
		else if (reg == 0x07) {
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
		else if (reg == 0x0D) {
			if (!BG1XScrollWritten) {
				BG1XScroll = value;
				BG1XScrollWritten = true;
			}
			else {
				BG1XScroll |= value << 8;
				BG1XScrollWritten = false;
			}
		}
		else if (reg == 0x0E) {
			if (!BG1YScrollWritten) {
				BG1YScroll = value;
				BG1YScrollWritten = true;
			}
			else {
				BG1YScroll |= value << 8;
				BG1YScrollWritten = false;
			}
		}
		else if (reg == 0x0F) {
			if (!BG2XScrollWritten) {
				BG2XScroll = value;
				BG2XScrollWritten = true;
			}
			else {
				BG2XScroll |= value << 8;
				BG2XScrollWritten = false;
			}
		}
		else if (reg == 0x10) {
			if (!BG2YScrollWritten) {
				BG2YScroll = value;
				BG2YScrollWritten = true;
			}
			else {
				BG2YScroll |= value << 8;
				BG2YScrollWritten = false;
			}
		}
		else if (reg == 0x11) {
			if (!BG3XScrollWritten) {
				BG3XScroll = value;
				BG3XScrollWritten = true;
			}
			else {
				BG3XScroll |= value << 8;
				BG3XScrollWritten = false;
			}
		}
		else if (reg == 0x12) {
			if (!BG3YScrollWritten) {
				BG3YScroll = value;
				BG3YScrollWritten = true;
			}
			else {
				BG3YScroll |= value << 8;
				BG3YScrollWritten = false;
			}
		}
		else if (reg == 0x13) {
			if (!BG4XScrollWritten) {
				BG4XScroll = value;
				BG4XScrollWritten = true;
			}
			else {
				BG4XScroll |= value << 8;
				BG4XScrollWritten = false;
			}
		}
		else if (reg == 0x14) {
			if (!BG4YScrollWritten) {
				BG4YScroll = value;
				BG4YScrollWritten = true;
			}
			else {
				BG4YScroll |= value << 8;
				BG4YScrollWritten = false;
			}
		}
		// VRAM
		else if (reg == 0x15) {	// Video Port Control
			VMain = value;
		}
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
			if ((VMain & 0x80) == 0) {
				VRAMAdd++;
			}
		}
		else if (reg == 0x19) {	// High Byte Write
			VRAM[VRAMAdd] |= value << 8;
			if ((VMain & 0x80) == 0x80) {
				VRAMAdd++;
			}
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
		else if (reg == 0x2C) {
			TM = value;
		}
	}
}

void PPU::SetPixel(int X, int Y)
{
	SetBGDrop(X, Y);
	// Render from back to front
	if (BG4Active && (TM & 0x8)) {
		SetBGPixel(X, Y, BGTileAdd4, 4, GetColorBPPForBG(4), (BGTileAdd4 & 0x1) ? 64 : 32, (BGTileAdd4 & 0x2) ? 64 : 32, BG4XScroll, BG4YScroll);
	}
	if (BG3Active && (TM & 0x4)) {
		SetBGPixel(X, Y, BGTileAdd3, 3, GetColorBPPForBG(3), (BGTileAdd3 & 0x1) ? 64 : 32, (BGTileAdd3 & 0x2) ? 64 : 32, BG3XScroll, BG3YScroll);
	}
	if (BG2Active && (TM & 0x2)) {
		SetBGPixel(X, Y, BGTileAdd2, 2, GetColorBPPForBG(2), (BGTileAdd2 & 0x1) ? 64 : 32, (BGTileAdd2 & 0x2) ? 64 : 32, BG2XScroll, BG2YScroll);
	}
	if (BG1Active && (TM & 0x1)) {
		SetBGPixel(X, Y, BGTileAdd1, 1, GetColorBPPForBG(1), (BGTileAdd1 & 0x1) ? 64 : 32, (BGTileAdd1 & 0x2) ? 64 : 32, BG1XScroll, BG1YScroll);
	}
}

void PPU::SetBGPixel(int X, int Y, uint8_t BGAdd, uint8_t BGIndex, BPP bpp, uint8_t XSize, uint8_t YSize, uint16_t XScroll, uint16_t YScroll)
{
	uint8_t mosaicSize = 0;
	if (mosaicReg & (0x1 << (BGIndex - 1)) && (mosaicReg & 0xF0) > 0x10) {
		mosaicSize = ((mosaicReg & 0xF0) >> 4);
	}

	int XScrolled = (X + XScroll) % (8 * XSize);
	int YScrolled = (Y + YScroll) % (8 * YSize);

	uint16_t tile = ((BGAdd & 0x7C) << 8)
		+ ((((XSize == 64) ? (YScrolled % 256) : YScrolled) / 8) << 5)
		+ ((XScrolled % 256) / 8)
		+ (XScrolled / 0x100) * 0x400
		+ (XSize / 64) * ((YScrolled / 0x100) * 0x800);
	uint16_t tileWord = VRAM[tile];

	uint16_t tileId = tileWord & 0x3ff;			//  mask bits that are for index
	uint8_t BGPaletteN = (tileWord >> 10) & 0b111;
	uint8_t BGPriority = (tileWord >> 13) & 1;	//  0 - lower, 1 - higher
	uint8_t BGFlipX = (tileWord >> 14) & 1;		//  0 - normal, 1 - mirror horizontally
	uint8_t BGFlipY = (tileWord >> 15) & 1;		//  0 - normal, 1 - mirror vertically

	uint8_t i = YScrolled % 8;
	uint8_t j = XScrolled % 8;
	uint8_t VShift = i + (-i + 7 - i) * BGFlipY;
	uint8_t HShift = (7 - j) + (2 * j - 7) * BGFlipX;
	uint16_t tileAddress = 0;

	uint16_t tileOffset = 0;
	if (BGIndex == 1) {
		tileOffset = (BGAdd1_2 & 0xF) << 12;
	}
	else if (BGIndex == 2) {
		tileOffset = (BGAdd1_2 & 0xF0) << 8;
	}
	else if (BGIndex == 3) {
		tileOffset = (BGAdd3_4 & 0xF) << 12;
	}
	else if (BGIndex == 4) {
		tileOffset = (BGAdd3_4 & 0xF0) << 8;
	}

	uint16_t pal = 0;
	
	if (bpp == BPP::BPP2) {
		tileAddress = tileOffset + tileId * 0x8 + VShift;
		pal = GetColor2BPP(tileAddress, BGPaletteN, HShift, BGIndex);
	}
	else if (bpp == BPP::BPP4) {
		tileAddress = tileOffset + tileId * 0x10 + VShift;
		pal = GetColor4BPP(tileAddress, BGPaletteN, HShift);
	}
	else if (bpp == BPP::BPP8) {
		tileAddress = tileOffset + tileId * 0x20 + VShift;
		pal = GetColor8BPP(tileAddress, BGPaletteN, HShift);
	}

	if ((pal & 0x8000) == 0) {
		if (mosaicSize > 1) {
			if (Y % mosaicSize == 0 && X % mosaicSize == 0) {
				pixels[(Y * width) + X] = pal;
			}
			else {
				uint16_t color = pixels[((Y - (Y % mosaicSize)) * width) + (X - (X % mosaicSize))];
				pixels[(Y * width) + X] = color;
			}
		}
		else {
			pixels[(Y * width) + X] = pal;
		}
	}

	if (started)
		if (freopen_s(&LOG, "Test.txt", "a", stdout) == NULL) {
			started = false;
			debug = true;
		}

	if (debug) {
		std::cout << std::setfill('0');
		std::cout << "Pixel: " << std::dec << X << ", " << Y << std::endl;
		std::cout << "Position: " << std::dec << X / 8 << ", " << Y / 8 << std::endl;
		std::cout << "Address: " << std::hex << std::setw(4) << (int)(tile * 2) << std::endl;
		std::cout << "Value: " << std::setw(4) << (int)tileWord << std::endl;
		std::cout << "Character: " << std::dec << (int)tileId << std::endl;
		std::cout << "Char Address: " << std::hex << std::setw(4) << (int)(tileAddress * 2) << std::endl;
		std::cout << "Palete: " << std::dec << (int)BGPaletteN << std::endl;
		std::cout << "Priority: " << std::dec << (int)BGPriority << std::endl;
		std::cout << "hFlip: " << std::dec << (int)BGFlipX << std::endl;
		std::cout << "vFlip: " << std::dec << (int)BGFlipY << std::endl;
		std::cout << std::endl;
	}
}

void PPU::SetBGDrop(int X, int Y)
{
	pixels[(Y * width) + X] = CGRAM[0];
}

uint16_t PPU::GetColor(uint8_t id, uint8_t tilePal)
{
	if (id == 0) {
		return CGRAM[0];
	}
	return CGRAM[id + tilePal * 4];
}

uint16_t PPU::GetColor2BPP(uint16_t charAdd, uint8_t pal, uint8_t HShift, int BGNumber)
{
	uint8_t BGLo = VRAM[charAdd] & 0xff;
	uint8_t BGHi = VRAM[charAdd] >> 8;
	uint8_t palIndex = ((BGLo >> HShift) & 1) + (2 * ((BGHi >> HShift) & 1));

	if (palIndex == 0) {
		return CGRAM[0] | 0x8000;
	}
	return CGRAM[palIndex + pal * 4 + (BGNumber - 1) * 32];
}

uint16_t PPU::GetColor4BPP(uint16_t charAdd, uint8_t pal, uint8_t HShift)
{
	uint8_t BGLo1 = VRAM[charAdd] & 0xff;
	uint8_t BGLo2 = VRAM[charAdd] >> 8;
	uint8_t BGHi1 = VRAM[charAdd + 8] & 0xff;
	uint8_t BGHi2 = VRAM[charAdd + 8] >> 8;
	uint8_t palIndex = ((BGLo1 >> HShift) & 1) + (2 * ((BGLo2 >> HShift) & 1)) + (4 * ((BGHi1 >> HShift) & 1)) + (8 * ((BGHi2 >> HShift) & 1));

	if (palIndex == 0) {
		return CGRAM[0];
	}
	return CGRAM[palIndex + pal * 16];
}

uint16_t PPU::GetColor8BPP(uint16_t charAdd, uint8_t pal, uint8_t HShift)
{
	uint8_t BG1 = VRAM[charAdd] & 0xff;
	uint8_t BG2 = VRAM[charAdd] >> 8;
	uint8_t BG3 = VRAM[charAdd + 8] & 0xff;
	uint8_t BG4 = VRAM[charAdd + 8] >> 8;
	uint8_t BG5 = VRAM[charAdd + 16] & 0xff;
	uint8_t BG6 = VRAM[charAdd + 16] >> 8;
	uint8_t BG7 = VRAM[charAdd + 24] & 0xff;
	uint8_t BG8 = VRAM[charAdd + 24] >> 8;
	uint8_t palIndex = ((BG1 >> HShift) & 1) + (2 * ((BG2 >> HShift) & 1)) + (4 * ((BG3 >> HShift) & 1)) + (8 * ((BG4 >> HShift) & 1)) +
		(16 * ((BG5 >> HShift) & 1)) + (32 * ((BG6 >> HShift) & 1)) + (64 * ((BG7 >> HShift) & 1)) + (128 * ((BG8 >> HShift) & 1));
	
	if (palIndex == 0) {
		return CGRAM[0];
	}
	return CGRAM[palIndex + pal * 0];
}

uint8_t PPU::GetMode()
{
	return BGMode & 0x7;
}

PPU::BPP PPU::GetColorBPPForBG(int BGIndex)
{
	uint8_t mode = GetMode();

	if (mode == 0) {
		return BPP::BPP2;
	}
	if (mode == 1) {
		if (BGIndex == 3) {
			return BPP::BPP2;
		}
		return BPP::BPP4;
	}
	if (mode == 2) {
		return BPP::BPP4;
	}
	if (mode == 3) {
		if (BGIndex == 1) {
			return BPP::BPP8;
		}
		return BPP::BPP4;
	}
	return BPP();
}

bool PPU::GetSizeBG(uint8_t BGNumber)
{
	if (BGMode & (0x08 << BGNumber)) {
		return true;
	}
	return false;
}

uint16_t* PPU::GetFrame()
{
	frameCompleted = false;
	return pixels;
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

void PPU::ToggleBG1(bool state)
{
	BG1Active = state;
}

void PPU::ToggleBG2(bool state)
{
	BG2Active = state;
}

void PPU::ToggleBG3(bool state)
{
	BG3Active = state;
}

void PPU::ToggleBG4(bool state)
{
	BG4Active = state;
}
