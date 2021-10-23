#include "Cartridge.h"

#include "Memory.h"

bool Cartridge::LoadRom(OPENFILENAME gameDir) {
	std::ifstream ifs;
	ifs.open(gameDir.lpstrFile, std::ifstream::binary);
	if (ifs.is_open()) {

		int fileSize = ifs.tellg();
		ifs.seekg(0, std::ios::end);
		fileSize = (int)ifs.tellg() - fileSize;
		std::cout << "Size: " << fileSize << " bytes" << std::endl;

		int offset = 0;
		// Check SMD header
		int remainder = fileSize % 1024;
		if (remainder == 512) {
			std::cout << "There is a SMD Header" << std::endl;
			offset = 512;
		}
		else if(remainder == 0) {
			std::cout << "There is not a SMD Header" << std::endl;
		}
		else {
			std::cout << "Invalid ROM" << std::endl;
			return false;
		}
		
		// Differ LoROM and HiROM
		int headerDir = 0x7FC0 + offset;
		int checksumDir = headerDir + 0x1C;
		ifs.seekg(checksumDir);
		uint16_t checksumCom = 0;
		uint16_t checksum = 0;
		ifs.read((char*)&checksumCom, sizeof(uint16_t));
		ifs.read((char*)&checksum, sizeof(uint16_t));
		if ((checksumCom ^ checksum) == 0xFFFF || fileSize < 0xFFC0) {
			std::cout << "Correct checksum. LoROM" << std::endl;
		}
		else {
			headerDir = 0xFFC0 + offset;
			checksumDir = headerDir + 0x1C;
			ifs.seekg(checksumDir);
			checksumCom = 0;
			checksum = 0;
			ifs.read((char*)&checksumCom, sizeof(uint16_t));
			ifs.read((char*)&checksum, sizeof(uint16_t));
			if ((checksumCom ^ checksum) == 0xFFFF) {
				std::cout << "Correct checksum. HiROM" << std::endl;
			}
			else {
				std::cout << "ROM type not handled but..." << std::endl;
				//return false;
				headerDir = 0x7FC0 + offset;
			}
		}

		// Header info
		ifs.seekg(headerDir);

		// Title
		char t;
		std::cout << "- ROM Title: ";
		for (int i = 0; i <= 0x14; i++) {
			ifs.read(&t, sizeof(char));
			std::cout << t;
		}
		std::cout << std::endl;
		// Cartridge Version 2
		if (t == 0) {
			std::cout << "Cartridge Ver. 2 not handled" << std::endl;
			return false;
		}

		// Mapping Mode and Speed
		uint8_t mapMode = 0;
		ifs.read((char*)&mapMode, sizeof(uint8_t));
		std::cout << "- Speed: ";
		if ((mapMode & 0x10) == 0x10) {
			std::cout << "FastROM" << std::endl;
		}
		else {
			std::cout << "SlowROM" << std::endl;
		}
		// TODO: check mapping mode

		// Cartridge type
		uint8_t cartType = 0;
		ifs.read((char*)&cartType, sizeof(uint8_t));
		std::cout << "- Chipset: ";
		std::cout << ROM_chipset_string.at(cartType) << std::endl;

		if ((cartType & 0x0F) > 0x2) {
			// TODO: coprocessors
			std::cout << "Coprocessor not handled: " << ROM_coprocessor_string.at(cartType >> 8) << std::endl;
			return false;
		}

		// ROM Size
		uint8_t romSize = 0;
		ifs.read((char*)&romSize, sizeof(uint8_t));
		std::cout << "- ROM Size: ";
		int romSizeByte = pow(2, romSize) * 1024;
		std::cout << romSizeByte << " bytes" << std::endl;

		// RAM Size
		uint8_t ramSize = 0;
		ifs.read((char*)&ramSize, sizeof(uint8_t));
		std::cout << "- RAM Size: ";
		int ramSizeByte = 0;
		if (ramSize != 0)
			ramSizeByte = pow(2, ramSize) * 1024;
		std::cout << ramSizeByte << " bytes" << std::endl;

		// Region
		uint8_t region = 0;
		ifs.read((char*)&region, sizeof(uint8_t));
		std::cout << "- Region: ";
		std::cout << Region.at(region) << std::endl;

		// Dev ID
		// Null read to step forward
		uint8_t devID = 0;
		ifs.read((char*)&devID, sizeof(uint8_t));
		if (devID == 0x33) {
			std::cout << "Cartridge Ver. 3 not handled" << std::endl;
			return false;
		}

		// Version
		uint8_t version = 0;
		ifs.read((char*)&version, sizeof(uint8_t));
		std::cout << "- Version: ";
		std::cout << "1." << (int)version << std::endl;

		// Checksums (we did this before, thus we skip it)
		uint32_t checksums = 0;
		ifs.read((char*)&checksums, sizeof(uint32_t));

		// TODO: Interrupt Vectors

		// TODO: Initialize memory
		ROM.resize(romSizeByte);
		ifs.seekg(offset);
		ifs.read((char*)ROM.data(), ROM.size());

		mem->SetRom(this);

		ifs.close();
	}

	return true;
}

uint8_t Cartridge::ReadRom(uint16_t add)
{
	uint8_t bank = (add & 0x00FF0000) >> 16;
	uint8_t page = (add & 0x0000FF00) >> 8;

	uint8_t value = 0;

	// Check bank range
	if (bank != 0x7E && bank != 0x7F) {

		//std::cout << (int)add << std::endl;
		// Check page range

		// Check top half
		if (page >= 0x80) {
			// ROM
			if (bank >= 0x80) {

			}
			// Mirror ROM
			else {
				value = ROM.at(add - 0x8000);
			}
		}
		// Check bottom half
		else if (page < 0x80) {
			// SRAM 2Q
			if (bank >= 0x40 && bank < 0x7E) {
				std::cout << "SRAM not handled" << std::endl;
			}
			// ROM bottom half 4Q
			else if (bank >= 0xC0 && bank < 0xF0) {

			}
			// SRAM 4Q
			else if (bank >= 0xF0) {
				std::cout << "SRAM not handled" << std::endl;
			}
		}
	}

	return value;
}
