#include "CPURicoh.h"

#include "Memory.h"

CPURicoh::CPURicoh()
{
	PC = 0x8000;

	A = 0;
	X = 0;
	Y = 0;

	SP = 0x01FF;

	DP = 0;

	PB = 0;

	DB = 0;

	P = 0x34;

	emulationMode = true;

	mem = nullptr;

}

FILE* LOG;
bool started = false;
int lines = 0;

int CPURicoh::Clock()
{
	if (!started) {
		if (freopen_s(&LOG, "TestADC.txt", "a", stdout) == NULL) {
			started = true;
		}
	}

	if (cycles == 0) {
		opcode = mem->ReadMemory((PB << 16) | PC++);
		//std::cout << std::hex << (int)opcode << std::endl;

		Debug();

		cycles = Execute();
	}
	
	cycles--;

	return 0;
}

void CPURicoh::Debug() {
	std::cout << std::setfill('0');
	std::cout << std::hex << std::setw(6) << (PC - 1) << " ";
	std::cout << "A:" << std::setw(4) << A << " ";
	std::cout << "X:" << std::setw(4) << X << " ";
	std::cout << "Y:" << std::setw(4) << Y << " ";
	std::cout << "S:" << std::setw(4) << SP << " ";
	std::cout << "D:" << std::setw(4) << DP << " ";
	std::cout << "DB:" << std::setw(2) << (int)DB << " ";

	if (P & NFlag) {
		std::cout << "N";
	}
	else {
		std::cout << "n";
	}

	if (P & VFlag) {
		std::cout << "V";
	}
	else {
		std::cout << "v";
	}

	if (emulationMode) {
		std::cout << "1B";
	}
	else {
		if (P & MFlag) {
			std::cout << "M";
		}
		else {
			std::cout << "m";
		}

		if (P & XFlag) {
			std::cout << "X";
		}
		else {
			std::cout << "x";
		}
	}

	if (P & DFlag) {
		std::cout << "D";
	}
	else {
		std::cout << "d";
	}

	if (P & IFlag) {
		std::cout << "I";
	}
	else {
		std::cout << "i";
	}

	if (P & ZFlag) {
		std::cout << "Z";
	}
	else {
		std::cout << "z";
	}

	if (P & CFlag) {
		std::cout << "C";
	}
	else {
		std::cout << "c";
	}

	lines++;

	if (lines >= 22845) {
		fclose(stdout);
		std::cout << " " << std::endl;
	}
	std::cout << " " << std::endl;
}

uint8_t CPURicoh::ReadMemory(uint32_t add, bool isLong) {
	if (isLong) {
		return mem->ReadMemory(add);
	}
	return mem->ReadMemory((DB << 16) | add);
}

void CPURicoh::WriteMemory(uint32_t add, uint8_t value, bool isLong) {
	if (isLong) {
		mem->WriteMemory(add, value);
	}
	else {
		mem->WriteMemory((DB << 16) | add, value);
	}
}

uint8_t CPURicoh::Pull() {
	return ReadMemory(++SP, true);
}

void CPURicoh::Push(uint8_t value) {
	WriteMemory(SP--, value, true);
}

uint8_t CPURicoh::ReadCPU(uint32_t add) {

	uint8_t value = 0;

	uint8_t page = (add & 0xFF00) >> 8;
	uint8_t reg = add & 0xFF;

	if (page == 0x42) {

		if (reg == 0x10) {
			if (NMI == 0x42) {
				NMI = 0xc2;
				value = NMI;
			}
			else if (NMI == 0xc2) {
				NMI = 0x42;
				value = NMI;
			}
		}

	}
	else if (page == 0x43) {
		std::cout << "DMA not handled" << std::endl;
	}
	//std::cout << std::hex << (int)value << std::endl;

	return value;
}

void CPURicoh::WriteCPU(uint32_t add, uint8_t value) {

}

int CPURicoh::Execute() {
	switch (opcode) {
		// BRK
	case 0x00:

		break;

		// ORA (dp, X)
	case 0x01:

		break;

		// COP const
	case 0x02:

		break;

		// ORA sr, S
	case 0x03:

		break;

		// TSB dp
	case 0x04:

		break;

		// ORA dp
	case 0x05:

		break;

		// ASL dp
	case 0x06:

		break;

		// ORA [dp]
	case 0x07:

		break;

		// PHP
	case 0x08:

		Push(P);

		return 3;

		// ORA #const
	case 0x09:

		break;

		// ASL A
	case 0x0A:

		break;

		// PHD
	case 0x0B:

		break;

		// TSB addr
	case 0x0C:

		break;

		// ORA addr
	case 0x0D:

		break;

		// ASL addr
	case 0x0E:

		break;

		// ORA long
	case 0x0F:

		break;

		// BPL nearlabel
	case 0x10: {

		int8_t offset = ReadMemory(PC++, false);

		if (!(P & NFlag)) {

			int c = 0;

			uint16_t page = PC & 0xFF00;
			PC += offset;
			if ((PC & 0xFF00) != page) {
				c = 1;
			}

			return 3 + c;

		}

		return 2;
	}
		// ORA (dp), Y
	case 0x11:

		break;

		// ORA (dp)
	case 0x12:

		break;

		// ORA (sr, S), Y
	case 0x13:

		break;

		// TRB dp
	case 0x14:

		break;

		// ORA dp, X
	case 0x15:

		break;

		// ASL dp, X
	case 0x16:

		break;

		// ORA [dp], Y
	case 0x17:

		break;

		// CLC
	case 0x18:

		P &= ~CFlag;

		return 2;

		// ORA addr, Y
	case 0x19:

		break;

		// INC A
	case 0x1A:

		A++;

		CheckNFlag(A, true, false);
		CheckZFlag(A, true, false);

		return 2;

		// TCS
	case 0x1B:

		break;

		// TRB addr
	case 0x1C:

		break;

		// ORA addr, X
	case 0x1D:

		break;

		// ASL addr, X
	case 0x1E:

		break;

		// ORA long, X
	case 0x1F:

		break;

		// JSR addr
	case 0x20: {

		uint16_t add = ReadMemory(PC++, false);
		add |= ReadMemory(PC++, false) << 8;

		Push((PC & 0xFF00) >> 8);
		Push(PC & 0xFF);

		PC = add;

		return 6;
	}
		// AND (dp, X)
	case 0x21:

		break;

		// JSR long
	case 0x22:

		break;

		// AND sr, S
	case 0x23:

		break;

		// BIT dp
	case 0x24: {

		uint32_t add = ReadMemory(PC++, false);

		add |= (DP << 8);

		uint16_t value = ReadMemory(add, true);
		uint16_t result;

		int c = 0;

		if (!(P & MFlag)) {
			c++;

			value |= ReadMemory(add + 1, true) << 8;

			if (value & 0x8000) {
				P |= NFlag;
			}
			else {
				P &= ~NFlag;
			}

			if (value & 0x4000) {
				P |= VFlag;
			}
			else {
				P &= ~VFlag;
			}

			result = A & value;
		}
		else {
			result = (A & 0xFF) & value;

			if (value & 0x80) {
				P |= NFlag;
			}
			else {
				P &= ~NFlag;
			}

			if (value & 0x40) {
				P |= VFlag;
			}
			else {
				P &= ~VFlag;
			}
		}

		if ((DP & 0xFF) != 0) {
			c++;
		}

		CheckZFlag(result, true, false);

		return 3 + c;
	}
		// AND dp
	case 0x25:

		break;

		// ROL dp
	case 0x26:

		break;

		// AND [dp]
	case 0x27:

		break;

		// PLP
	case 0x28:

		break;

		// AND #const
	case 0x29: {

		uint16_t value = ReadMemory(PC++, false);

		if (!(P & MFlag)) {
			value |= ReadMemory(PC++, false) << 8;

			A &= value;

			if (A == 0) {
				P |= ZFlag;
			}
			else {
				P &= ~ZFlag;
			}

			if (A & 0x8000) {
				P |= NFlag;
			}
			else {
				P &= ~NFlag;
			}
		}
		else {
			A &= (value | 0xFF00);

			if ((A & 0xFF) == 0) {
				P |= ZFlag;
			}
			else {
				P &= ~ZFlag;
			}

			if (A & 0x80) {
				P |= NFlag;
			}
			else {
				P &= ~NFlag;
			}
		}

		return !(P & MFlag) ? 3 : 2;
	}
		// ROL A
	case 0x2A:

		break;

		// PLD
	case 0x2B:

		break;

		// BIT addr
	case 0x2C: {

		uint16_t add = ReadMemory(PC++, false);
		add |= ReadMemory(PC++, false) << 8;

		uint16_t value = ReadMemory(add, false);
		uint16_t result;

		if (!(P & MFlag)) {
			value |= ReadMemory(add + 1, false) << 8;

			if (value & 0x8000) {
				P |= NFlag;
			}
			else {
				P &= ~NFlag;
			}

			if (value & 0x4000) {
				P |= VFlag;
			}
			else {
				P &= ~VFlag;
			}

			result = A & value;
		}
		else {
			result = (A & 0xFF) & value;

			if (value & 0x80) {
				P |= NFlag;
			}
			else {
				P &= ~NFlag;
			}

			if (value & 0x40) {
				P |= VFlag;
			}
			else {
				P &= ~VFlag;
			}
		}

		CheckZFlag(result, true, false);

		return !(P & MFlag) ? 5 : 4;
	}
		// AND addr
	case 0x2D:

		break;

		// ROL addr
	case 0x2E:

		break;

		// AND long
	case 0x2F:

		break;

		// BMI nearlabel
	case 0x30:

		break;

		// AND (dp), Y
	case 0x31:

		break;

		// AND (dp)
	case 0x32:

		break;

		// AND (sr, S), Y
	case 0x33:

		break;

		// BIT dp, X
	case 0x34:

		break;

		// AND dp, X
	case 0x35:

		break;

		// ROL dp, X
	case 0x36:

		break;

		// AND [dp], Y
	case 0x37:

		break;

		// SEC
	case 0x38:

		P |= CFlag;

		return 2;

		// AND addr, Y
	case 0x39:

		break;

		// DEC A
	case 0x3A:

		A--;

		CheckNFlag(A, true, false);
		CheckZFlag(A, true, false);

		return 2;

		// TSC
	case 0x3B:

		break;

		// BIT addr, X
	case 0x3C:

		break;

		// AND addr, X
	case 0x3D:

		break;

		// ROL addr, X
	case 0x3E:

		break;

		// AND long, X
	case 0x3F:

		break;

		// RTI
	case 0x40:

		break;

		// EOR (dp, X)
	case 0x41:

		break;

		// WDM
	case 0x42:

		std::cout << "WDM" << std::endl;
		fclose(stdout);
		std::cout << " " << std::endl;

		return 0;

		// EOR sr, S
	case 0x43:

		break;

		// MVP srcbk, destbk
	case 0x44:

		break;

		// EOR dp
	case 0x45:

		break;

		// LSR dp
	case 0x46:

		break;

		// EOR [dp]
	case 0x47:

		break;

		// PHA
	case 0x48:

		if (!(P & MFlag)) {
			Push((A & 0xFF00) >> 8);
		}
		Push(A & 0xFF);

		return !(P & MFlag) ? 4 : 3;

		// EOR #const
	case 0x49:

		break;

		// LSR A
	case 0x4A: {

		uint16_t prevA = A;

		bool M = P & MFlag;

		if (M) {
			uint8_t l = A & 0xFF;
			A &= 0xFF00;
			A |= l >> 1;
		}
		else {
			A >>= 1;
		}

		if ((M && ((A & 0xFF) == 0)) || (!M && (A == 0))) {
			P |= ZFlag;
		}
		else {
			P &= ~ZFlag;
		}

		if ((M && ((A & 0xFF) & 0x80)) || (!M && (A & 0x8000))) {
			P |= NFlag;
		}
		else {
			P &= ~NFlag;
		}

		if (prevA & 0x1) {
			P |= CFlag;
		}
		else {
			P &= ~CFlag;
		}

		return 2;
	}
		// PHK
	case 0x4B:

		Push(PB);

		return 3;

		// JMP addr
	case 0x4C:

		break;

		// EOR addr
	case 0x4D:

		break;

		// LSR addr
	case 0x4E:

		break;

		// EOR long
	case 0x4F:

		break;

		// BVC nearlabel
	case 0x50:

		break;

		// EOR (dp), Y
	case 0x51:

		break;

		// EOR (dp)
	case 0x52:

		break;

		// EOR (sr, S), Y
	case 0x53:

		break;

		// MVN srcbk, destbk
	case 0x54:

		break;

		// EOR dp, X
	case 0x55:

		break;

		// LSR dp, X
	case 0x56:

		break;

		// EOR [dp], Y
	case 0x57:

		break;

		// CLI
	case 0x58:

		P &= ~IFlag;

		return 2;

		// EOR addr, Y
	case 0x59:

		break;

		// PHY
	case 0x5A:

		if (!(P & XFlag)) {
			Push((Y & 0xFF00) >> 8);
		}
		Push(Y & 0xFF);

		return !(P & XFlag) ? 4 : 3;

		// TCD
	case 0x5B:

		DP = A;

		CheckNFlag(DP, false, false);
		CheckZFlag(DP, false ,false);

		return 2;

		// JMP long
	case 0x5C: {

		uint16_t add = ReadMemory(PC++, false);
		add |= ReadMemory(PC++, false) << 8;
		PB = ReadMemory(PC++, false);

		PC = add;

		return 4;
	}
		// EOR addr, X
	case 0x5D:

		break;

		// LSR addr, X
	case 0x5E:

		break;

		// EOR long, X
	case 0x5F:

		break;

		// RST
	case 0x60: {

		PC = Pull();
		PC |= Pull() << 8;

		return 6;
	}
		// ADC (dp, X)
	case 0x61: {

		uint16_t prevA;

		uint16_t indAdd = ReadMemory(PC++, false);
		indAdd |= DP << 8;
		indAdd += X;

		int c = 0;
		if (DP & 0xFF) {
			c++;
		}

		uint16_t add = ReadMemory(indAdd, true);
		add |= ReadMemory(indAdd + 1, true) << 8;

		int16_t value = ReadMemory(add, true);

		int8_t C = (P & CFlag) ? 1 : 0;

		if (P & MFlag) {
			uint16_t result;
			// Decimal Mode
			if (P & DFlag) {
				result = (A & 0x0F) + (uint8_t)(value & 0x0F) + C;
				if (result > 0x09) {
					result += 0x06;
				}
				int c = (result > 0x0F);
				result = (A & 0xF0) + (uint8_t)(value & 0xF0) + (c << 4) + (result & 0x0F);

				prevA = A;

				if (~((prevA & 0xFF) ^ value) & ((prevA & 0xFF) ^ result) & 0x80) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}

				if (result > 0x9F) {
					result += 0x60;
				}

				A &= 0xFF00;
				A |= (uint8_t)result;
			}
			else {
				prevA = A;

				result = (A & 0xFF) + (uint8_t)value + C;

				A &= 0xFF00;
				A |= (uint8_t)result;

				if (~((prevA & 0xFF) ^ value) & ((prevA & 0xFF) ^ result) & 0x80) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}
			}
		}
		else {
			c++;
			if (P & DFlag) {
				uint16_t result;
				value |= ReadMemory(add + 1, true) << 8;

				result = (A & 0x0F) + (value & 0x0F) + C;
				if (result > 0x09) {
					result += 0x06;
				}
				int c = (result > 0x0F);
				result = (A & 0xF0) + (value & 0xF0) + (c << 4) + (result & 0x0F);

				if (result > 0x9F) {
					result += 0x60;
				}
				c = (result > 0x0FF);
				result = (A & 0xF00) + (value & 0xF00) + (c << 8) + (result & 0xFF);

				if (result > 0x9FF) {
					result += 0x600;
				}
				c = (result > 0x0FFF);
				result = (A & 0xF000) + (value & 0xF000) + (c << 12) + (result & 0xFFF);

				prevA = A;

				if (~(prevA ^ value) & (prevA ^ result) & 0x8000) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}

				if (result > 0x9FFF) {
					result += 0x6000;
				}

				A = result;
			}
			else {
				prevA = (int16_t)A;
				value |= ReadMemory(add + 1, false) << 8;

				A = (int16_t)A + (int16_t)value + C;

				if (~(prevA ^ value) & (prevA ^ A) & 0x8000) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}
			}
		}

		CheckNFlag(A, true, false);

		CheckZFlag(A, true, false);
		CheckCFlag(A, prevA, false);

		return 6 + c;
	}
		// PER label
	case 0x62:

		break;

		// ADC sr, S
	case 0x63: {

		uint16_t prevA;

		uint16_t add = ReadMemory(PC++, false);
		add += SP;

		int16_t value = ReadMemory(add, true);

		int8_t C = (P & CFlag) ? 1 : 0;

		if (P & MFlag) {
			uint16_t result;
			// Decimal Mode
			if (P & DFlag) {
				result = (A & 0x0F) + (uint8_t)(value & 0x0F) + C;
				if (result > 0x09) {
					result += 0x06;
				}
				C = (result > 0x0F);
				result = (A & 0xF0) + (uint8_t)(value & 0xF0) + (C << 4) + (result & 0x0F);

				prevA = A;

				if (~((prevA & 0xFF) ^ value) & ((prevA & 0xFF) ^ result) & 0x80) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}

				if (result > 0x9F) {
					result += 0x60;
				}

				A &= 0xFF00;
				A |= (uint8_t)result;
			}
			else {
				prevA = A;

				result = (A & 0xFF) + (uint8_t)value + C;

				A &= 0xFF00;
				A |= (uint8_t)result;

				if (~((prevA & 0xFF) ^ value) & ((prevA & 0xFF) ^ result) & 0x80) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}
			}
		}
		else {
			if (P & DFlag) {
				uint16_t result;
				value |= ReadMemory(add + 1, true) << 8;

				result = (A & 0x0F) + (value & 0x0F) + C;
				if (result > 0x09) {
					result += 0x06;
				}
				C = (result > 0x0F);
				result = (A & 0xF0) + (value & 0xF0) + (C << 4) + (result & 0x0F);

				if (result > 0x9F) {
					result += 0x60;
				}
				C = (result > 0x0FF);
				result = (A & 0xF00) + (value & 0xF00) + (C << 8) + (result & 0xFF);

				if (result > 0x9FF) {
					result += 0x600;
				}
				C = (result > 0x0FFF);
				result = (A & 0xF000) + (value & 0xF000) + (C << 12) + (result & 0xFFF);

				prevA = A;

				if (~(prevA ^ value) & (prevA ^ result) & 0x8000) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}

				if (result > 0x9FFF) {
					result += 0x6000;
				}

				A = result;
			}
			else {
				prevA = (int16_t)A;
				value |= ReadMemory(add + 1, true) << 8;

				A = (int16_t)A + (int16_t)value + C;

				if (~(prevA ^ value) & (prevA ^ A) & 0x8000) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}
			}
		}

		CheckNFlag(A, true, false);

		CheckZFlag(A, true, false);
		CheckCFlag(A, prevA, false);

		return !(P & MFlag) ? 5 : 4;
	}
		// STZ dp
	case 0x64:

		break;

		// ADC dp
	case 0x65: {

		uint16_t prevA;

		uint16_t add = ReadMemory(PC++, false);

		add |= DP << 8;

		int16_t value = ReadMemory(add, true);

		int8_t C = (P & CFlag) ? 1 : 0;

		int c = 0;

		if (DP & 0xFF) {
			c++;
		}

		if (P & MFlag) {
			uint16_t result;
			// Decimal Mode
			if (P & DFlag) {
				result = (A & 0x0F) + (uint8_t)(value & 0x0F) + C;
				if (result > 0x09) {
					result += 0x06;
				}
				int c = (result > 0x0F);
				result = (A & 0xF0) + (uint8_t)(value & 0xF0) + (c << 4) + (result & 0x0F);

				prevA = A;

				if (~((prevA & 0xFF) ^ value) & ((prevA & 0xFF) ^ result) & 0x80) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}

				if (result > 0x9F) {
					result += 0x60;
				}

				A &= 0xFF00;
				A |= (uint8_t)result;
			}
			else {
				prevA = A;

				result = (A & 0xFF) + (uint8_t)value + C;

				A &= 0xFF00;
				A |= (uint8_t)result;

				if (~((prevA & 0xFF) ^ value) & ((prevA & 0xFF) ^ result) & 0x80) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}
			}
		}
		else {
			if (P & DFlag) {
				uint16_t result;
				value |= ReadMemory(add + 1, true) << 8;

				result = (A & 0x0F) + (value & 0x0F) + C;
				if (result > 0x09) {
					result += 0x06;
				}
				int c = (result > 0x0F);
				result = (A & 0xF0) + (value & 0xF0) + (c << 4) + (result & 0x0F);

				if (result > 0x9F) {
					result += 0x60;
				}
				c = (result > 0x0FF);
				result = (A & 0xF00) + (value & 0xF00) + (c << 8) + (result & 0xFF);

				if (result > 0x9FF) {
					result += 0x600;
				}
				c = (result > 0x0FFF);
				result = (A & 0xF000) + (value & 0xF000) + (c << 12) + (result & 0xFFF);

				prevA = A;

				if (~(prevA ^ value) & (prevA ^ result) & 0x8000) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}

				if (result > 0x9FFF) {
					result += 0x6000;
				}

				A = result;
			}
			else {
				prevA = (int16_t)A;
				value |= ReadMemory(add + 1, false) << 8;

				A = (int16_t)A + (int16_t)value + C;

				if (~(prevA ^ value) & (prevA ^ A) & 0x8000) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}
			}
		}

		CheckNFlag(A, true, false);

		CheckZFlag(A, true, false);
		CheckCFlag(A, prevA, false);

		return 3 + c;
	}
		// ROR dp
	case 0x66:

		break;

		// ADC [dp]
	case 0x67: {

		uint16_t prevA;

		uint16_t indAdd = ReadMemory(PC++, false);
		indAdd |= DP << 8;

		int c = 0;
		if (DP & 0xFF) {
			c++;
		}

		uint32_t add = ReadMemory(indAdd, true);
		add |= ReadMemory(indAdd + 1, true) << 8;
		add |= ReadMemory(indAdd + 2, true) << 16;

		int16_t value = ReadMemory(add, true);

		int8_t C = (P & CFlag) ? 1 : 0;

		if (P & MFlag) {
			uint16_t result;
			// Decimal Mode
			if (P & DFlag) {
				result = (A & 0x0F) + (uint8_t)(value & 0x0F) + C;
				if (result > 0x09) {
					result += 0x06;
				}
				int c = (result > 0x0F);
				result = (A & 0xF0) + (uint8_t)(value & 0xF0) + (c << 4) + (result & 0x0F);

				prevA = A;

				if (~((prevA & 0xFF) ^ value) & ((prevA & 0xFF) ^ result) & 0x80) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}

				if (result > 0x9F) {
					result += 0x60;
				}

				A &= 0xFF00;
				A |= (uint8_t)result;
			}
			else {
				prevA = A;

				result = (A & 0xFF) + (uint8_t)value + C;

				A &= 0xFF00;
				A |= (uint8_t)result;

				if (~((prevA & 0xFF) ^ value) & ((prevA & 0xFF) ^ result) & 0x80) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}
			}
		}
		else {
			c++;
			if (P & DFlag) {
				uint16_t result;
				value |= ReadMemory(add + 1, true) << 8;

				result = (A & 0x0F) + (value & 0x0F) + C;
				if (result > 0x09) {
					result += 0x06;
				}
				int c = (result > 0x0F);
				result = (A & 0xF0) + (value & 0xF0) + (c << 4) + (result & 0x0F);

				if (result > 0x9F) {
					result += 0x60;
				}
				c = (result > 0x0FF);
				result = (A & 0xF00) + (value & 0xF00) + (c << 8) + (result & 0xFF);

				if (result > 0x9FF) {
					result += 0x600;
				}
				c = (result > 0x0FFF);
				result = (A & 0xF000) + (value & 0xF000) + (c << 12) + (result & 0xFFF);

				prevA = A;

				if (~(prevA ^ value) & (prevA ^ result) & 0x8000) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}

				if (result > 0x9FFF) {
					result += 0x6000;
				}

				A = result;
			}
			else {
				prevA = (int16_t)A;
				value |= ReadMemory(add + 1, false) << 8;

				A = (int16_t)A + (int16_t)value + C;

				if (~(prevA ^ value) & (prevA ^ A) & 0x8000) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}
			}
		}

		CheckNFlag(A, true, false);

		CheckZFlag(A, true, false);
		CheckCFlag(A, prevA, false);

		return 6 + c;
	}
		// PLA
	case 0x68:

		A &= 0xFF00;
		A |= Pull();

		if (!(P & MFlag)) {
			A &= 0x00FF;
			A |= Pull() << 8;

			if (A == 0) {
				P |= ZFlag;
			}
			else {
				P &= ~ZFlag;
			}

			if (A & 0x8000) {
				P |= NFlag;
			}
			else {
				P &= ~NFlag;
			}
		}
		else {
			if ((A & 0xFF) == 0) {
				P |= ZFlag;
			}
			else {
				P &= ~ZFlag;
			}

			if (A & 0x80) {
				P |= NFlag;
			}
			else {
				P &= ~NFlag;
			}
		}

		return !(P & MFlag) ? 5 : 4;

		// ADC #const
	case 0x69: {

		uint16_t prevA;

		int16_t value;

		value = ReadMemory(PC++, false);

		int8_t C = (P & CFlag) ? 1 : 0;

		if (P & MFlag) {
			uint16_t result;
			// Decimal Mode
			if (P & DFlag) {
				result = (A & 0x0F) + (uint8_t)(value & 0x0F) + C;
				if (result > 0x09) {
					result += 0x06;
				}
				int c = (result > 0x0F);
				result = (A & 0xF0) + (uint8_t)(value & 0xF0) + (c << 4) + (result & 0x0F);

				prevA = A;

				if (~((prevA & 0xFF) ^ value) & ((prevA & 0xFF) ^ result) & 0x80) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}

				if (result > 0x9F) {
					result += 0x60;
				}

				A &= 0xFF00;
				A |= (uint8_t)result;
			}
			else {
				prevA = A;

				result = (A & 0xFF) + (uint8_t)value + C;

				A &= 0xFF00;
				A |= (uint8_t)result;

				if (~((prevA & 0xFF) ^ value) & ((prevA & 0xFF) ^ result) & 0x80) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}
			}
		}
		else {
			if (P & DFlag) {
				uint16_t result;
				value |= ReadMemory(PC++, false) << 8;

				result = (A & 0x0F) + (value & 0x0F) + C;
				if (result > 0x09) {
					result += 0x06;
				}
				int c = (result > 0x0F);
				result = (A & 0xF0) + (value & 0xF0) + (c << 4) + (result & 0x0F);

				if (result > 0x9F) {
					result += 0x60;
				}
				c = (result > 0x0FF);
				result = (A & 0xF00) + (value & 0xF00) + (c << 8) + (result & 0xFF);

				if (result > 0x9FF) {
					result += 0x600;
				}
				c = (result > 0x0FFF);
				result = (A & 0xF000) + (value & 0xF000) + (c << 12) + (result & 0xFFF);

				prevA = A;

				if (~(prevA ^ value) & (prevA ^ result) & 0x8000) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}

				if (result > 0x9FFF) {
					result += 0x6000;
				}

				A = result;
			}
			else {
				prevA = (int16_t)A;
				value |= ReadMemory(PC++, false) << 8;

				A = (int16_t)A + (int16_t)value + C;

				if (~(prevA ^ value) & (prevA ^ A) & 0x8000) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}
			}
		}

		CheckNFlag(A, true, false);
		
		CheckZFlag(A, true, false);
		CheckCFlag(A, prevA, false);

		return !(P & MFlag) ? 3 : 2;
	}
		// ROR A
	case 0x6A:

		break;

		// RTL
	case 0x6B:

		break;

		// JMP (addr)
	case 0x6C:

		break;

		// ADC addr
	case 0x6D: {

		uint16_t prevA;

		uint16_t add = ReadMemory(PC++, false);
		add |= ReadMemory(PC++, false) << 8;

		int16_t value = ReadMemory(add, false);

		int8_t C = (P & CFlag) ? 1 : 0;

		if (P & MFlag) {
			uint16_t result;
			// Decimal Mode
			if (P & DFlag) {
				result = (A & 0x0F) + (uint8_t)(value & 0x0F) + C;
				if (result > 0x09) {
					result += 0x06;
				}
				C = (result > 0x0F);
				result = (A & 0xF0) + (uint8_t)(value & 0xF0) + (C << 4) + (result & 0x0F);

				prevA = A;

				if (~((prevA & 0xFF) ^ value) & ((prevA & 0xFF) ^ result) & 0x80) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}

				if (result > 0x9F) {
					result += 0x60;
				}

				A &= 0xFF00;
				A |= (uint8_t)result;
			}
			else {
				prevA = A;

				result = (A & 0xFF) + (uint8_t)value + C;

				A &= 0xFF00;
				A |= (uint8_t)result;

				if (~((prevA & 0xFF) ^ value) & ((prevA & 0xFF) ^ result) & 0x80) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}
			}
		}
		else {
			if (P & DFlag) {
				uint16_t result;
				value |= ReadMemory(add + 1, false) << 8;

				result = (A & 0x0F) + (value & 0x0F) + C;
				if (result > 0x09) {
					result += 0x06;
				}
				C = (result > 0x0F);
				result = (A & 0xF0) + (value & 0xF0) + (C << 4) + (result & 0x0F);

				if (result > 0x9F) {
					result += 0x60;
				}
				C = (result > 0x0FF);
				result = (A & 0xF00) + (value & 0xF00) + (C << 8) + (result & 0xFF);

				if (result > 0x9FF) {
					result += 0x600;
				}
				C = (result > 0x0FFF);
				result = (A & 0xF000) + (value & 0xF000) + (C << 12) + (result & 0xFFF);

				prevA = A;

				if (~(prevA ^ value) & (prevA ^ result) & 0x8000) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}

				if (result > 0x9FFF) {
					result += 0x6000;
				}

				A = result;
			}
			else {
				prevA = (int16_t)A;
				value |= ReadMemory(add + 1, false) << 8;

				A = (int16_t)A + (int16_t)value + C;

				if (~(prevA ^ value) & (prevA ^ A) & 0x8000) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}
			}
		}

		CheckNFlag(A, true, false);

		CheckZFlag(A, true, false);
		CheckCFlag(A, prevA, false);

		return !(P & MFlag) ? 5 : 4;
	}
		// ROR addr
	case 0x6E:

		break;

		// ADC long
	case 0x6F: {

		uint16_t prevA;

		uint32_t add = ReadMemory(PC++, false);
		add |= ReadMemory(PC++, false) << 8;
		add |= ReadMemory(PC++, false) << 16;

		int16_t value = ReadMemory(add, true);

		int8_t C = (P & CFlag) ? 1 : 0;

		if (P & MFlag) {
			uint16_t result;
			// Decimal Mode
			if (P & DFlag) {
				result = (A & 0x0F) + (uint8_t)(value & 0x0F) + C;
				if (result > 0x09) {
					result += 0x06;
				}
				int c = (result > 0x0F);
				result = (A & 0xF0) + (uint8_t)(value & 0xF0) + (c << 4) + (result & 0x0F);

				prevA = A;

				if (~((prevA & 0xFF) ^ value) & ((prevA & 0xFF) ^ result) & 0x80) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}

				if (result > 0x9F) {
					result += 0x60;
				}

				A &= 0xFF00;
				A |= (uint8_t)result;
			}
			else {
				prevA = A;

				result = (A & 0xFF) + (uint8_t)value + C;

				A &= 0xFF00;
				A |= (uint8_t)result;

				if (~((prevA & 0xFF) ^ value) & ((prevA & 0xFF) ^ result) & 0x80) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}
			}
		}
		else {
			if (P & DFlag) {
				uint16_t result;
				value |= ReadMemory(add + 1, true) << 8;

				result = (A & 0x0F) + (value & 0x0F) + C;
				if (result > 0x09) {
					result += 0x06;
				}
				int c = (result > 0x0F);
				result = (A & 0xF0) + (value & 0xF0) + (c << 4) + (result & 0x0F);

				if (result > 0x9F) {
					result += 0x60;
				}
				c = (result > 0x0FF);
				result = (A & 0xF00) + (value & 0xF00) + (c << 8) + (result & 0xFF);

				if (result > 0x9FF) {
					result += 0x600;
				}
				c = (result > 0x0FFF);
				result = (A & 0xF000) + (value & 0xF000) + (c << 12) + (result & 0xFFF);

				prevA = A;

				if (~(prevA ^ value) & (prevA ^ result) & 0x8000) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}

				if (result > 0x9FFF) {
					result += 0x6000;
				}

				A = result;
			}
			else {
				prevA = (int16_t)A;
				value |= ReadMemory(add + 1, false) << 8;

				A = (int16_t)A + (int16_t)value + C;

				if (~(prevA ^ value) & (prevA ^ A) & 0x8000) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}
			}
		}

		CheckNFlag(A, true, false);

		CheckZFlag(A, true, false);
		CheckCFlag(A, prevA, false);

		return !(P & MFlag) ? 6 : 5;
	}
		// BVS nearlabel
	case 0x70:

		break;

		// ADC (dp), Y
	case 0x71: {

		uint16_t prevA;

		uint16_t indAdd = ReadMemory(PC++, false);
		indAdd |= DP << 8;

		int c = 0;
		if (DP & 0xFF) {
			c++;
		}

		uint16_t add = ReadMemory(indAdd, true);
		add |= ReadMemory(indAdd + 1, true) << 8;
		
		if ((P & XFlag) || ((add & 0xFF00) != ((add + Y) & 0xFF00))) {
			c++;
		}
		add += Y;

		int16_t value = ReadMemory(add, true);

		int8_t C = (P & CFlag) ? 1 : 0;

		if (P & MFlag) {
			uint16_t result;
			// Decimal Mode
			if (P & DFlag) {
				result = (A & 0x0F) + (uint8_t)(value & 0x0F) + C;
				if (result > 0x09) {
					result += 0x06;
				}
				C = (result > 0x0F);
				result = (A & 0xF0) + (uint8_t)(value & 0xF0) + (C << 4) + (result & 0x0F);

				prevA = A;

				if (~((prevA & 0xFF) ^ value) & ((prevA & 0xFF) ^ result) & 0x80) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}

				if (result > 0x9F) {
					result += 0x60;
				}

				A &= 0xFF00;
				A |= (uint8_t)result;
			}
			else {
				prevA = A;

				result = (A & 0xFF) + (uint8_t)value + C;

				A &= 0xFF00;
				A |= (uint8_t)result;

				if (~((prevA & 0xFF) ^ value) & ((prevA & 0xFF) ^ result) & 0x80) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}
			}
		}
		else {
			c++;
			if (P & DFlag) {
				uint16_t result;
				value |= ReadMemory(add + 1, true) << 8;

				result = (A & 0x0F) + (value & 0x0F) + C;
				if (result > 0x09) {
					result += 0x06;
				}
				C = (result > 0x0F);
				result = (A & 0xF0) + (value & 0xF0) + (C << 4) + (result & 0x0F);

				if (result > 0x9F) {
					result += 0x60;
				}
				C = (result > 0x0FF);
				result = (A & 0xF00) + (value & 0xF00) + (C << 8) + (result & 0xFF);

				if (result > 0x9FF) {
					result += 0x600;
				}
				C = (result > 0x0FFF);
				result = (A & 0xF000) + (value & 0xF000) + (C << 12) + (result & 0xFFF);

				prevA = A;

				if (~(prevA ^ value) & (prevA ^ result) & 0x8000) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}

				if (result > 0x9FFF) {
					result += 0x6000;
				}

				A = result;
			}
			else {
				prevA = (int16_t)A;
				value |= ReadMemory(add + 1, false) << 8;

				A = (int16_t)A + (int16_t)value + C;

				if (~(prevA ^ value) & (prevA ^ A) & 0x8000) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}
			}
		}

		CheckNFlag(A, true, false);

		CheckZFlag(A, true, false);
		CheckCFlag(A, prevA, false);

		return 6 + c;
	}
		// ADC (dp)
	case 0x72: {

		uint16_t prevA;

		uint16_t indAdd = ReadMemory(PC++, false);
		indAdd |= DP << 8;

		int c = 0;
		if (DP & 0xFF) {
			c++;
		}

		uint16_t add = ReadMemory(indAdd, true);
		add |= ReadMemory(indAdd + 1, true) << 8;

		int16_t value = ReadMemory(add, true);

		int8_t C = (P & CFlag) ? 1 : 0;

		if (P & MFlag) {
			uint16_t result;
			// Decimal Mode
			if (P & DFlag) {
				result = (A & 0x0F) + (uint8_t)(value & 0x0F) + C;
				if (result > 0x09) {
					result += 0x06;
				}
				int c = (result > 0x0F);
				result = (A & 0xF0) + (uint8_t)(value & 0xF0) + (c << 4) + (result & 0x0F);

				prevA = A;

				if (~((prevA & 0xFF) ^ value) & ((prevA & 0xFF) ^ result) & 0x80) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}

				if (result > 0x9F) {
					result += 0x60;
				}

				A &= 0xFF00;
				A |= (uint8_t)result;
			}
			else {
				prevA = A;

				result = (A & 0xFF) + (uint8_t)value + C;

				A &= 0xFF00;
				A |= (uint8_t)result;

				if (~((prevA & 0xFF) ^ value) & ((prevA & 0xFF) ^ result) & 0x80) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}
			}
		}
		else {
			c++;
			if (P & DFlag) {
				uint16_t result;
				value |= ReadMemory(add + 1, true) << 8;

				result = (A & 0x0F) + (value & 0x0F) + C;
				if (result > 0x09) {
					result += 0x06;
				}
				int c = (result > 0x0F);
				result = (A & 0xF0) + (value & 0xF0) + (c << 4) + (result & 0x0F);

				if (result > 0x9F) {
					result += 0x60;
				}
				c = (result > 0x0FF);
				result = (A & 0xF00) + (value & 0xF00) + (c << 8) + (result & 0xFF);

				if (result > 0x9FF) {
					result += 0x600;
				}
				c = (result > 0x0FFF);
				result = (A & 0xF000) + (value & 0xF000) + (c << 12) + (result & 0xFFF);

				prevA = A;

				if (~(prevA ^ value) & (prevA ^ result) & 0x8000) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}

				if (result > 0x9FFF) {
					result += 0x6000;
				}

				A = result;
			}
			else {
				prevA = (int16_t)A;
				value |= ReadMemory(add + 1, false) << 8;

				A = (int16_t)A + (int16_t)value + C;

				if (~(prevA ^ value) & (prevA ^ A) & 0x8000) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}
			}
		}

		CheckNFlag(A, true, false);

		CheckZFlag(A, true, false);
		CheckCFlag(A, prevA, false);

		return 5 + c;
	}
		// ADC (sr, S), Y
	case 0x73: {

		uint16_t prevA;

		uint16_t indAdd = ReadMemory(PC++, false);
		indAdd += SP;

		uint16_t add = ReadMemory(indAdd, true);
		add |= ReadMemory(indAdd + 1, true) << 8;
		add += Y;

		int16_t value = ReadMemory(add, true);

		int8_t C = (P & CFlag) ? 1 : 0;

		if (P & MFlag) {
			uint16_t result;
			// Decimal Mode
			if (P & DFlag) {
				result = (A & 0x0F) + (uint8_t)(value & 0x0F) + C;
				if (result > 0x09) {
					result += 0x06;
				}
				C = (result > 0x0F);
				result = (A & 0xF0) + (uint8_t)(value & 0xF0) + (C << 4) + (result & 0x0F);

				prevA = A;

				if (~((prevA & 0xFF) ^ value) & ((prevA & 0xFF) ^ result) & 0x80) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}

				if (result > 0x9F) {
					result += 0x60;
				}

				A &= 0xFF00;
				A |= (uint8_t)result;
			}
			else {
				prevA = A;

				result = (A & 0xFF) + (uint8_t)value + C;

				A &= 0xFF00;
				A |= (uint8_t)result;

				if (~((prevA & 0xFF) ^ value) & ((prevA & 0xFF) ^ result) & 0x80) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}
			}
		}
		else {
			if (P & DFlag) {
				uint16_t result;
				value |= ReadMemory(add + 1, true) << 8;

				result = (A & 0x0F) + (value & 0x0F) + C;
				if (result > 0x09) {
					result += 0x06;
				}
				C = (result > 0x0F);
				result = (A & 0xF0) + (value & 0xF0) + (C << 4) + (result & 0x0F);

				if (result > 0x9F) {
					result += 0x60;
				}
				C = (result > 0x0FF);
				result = (A & 0xF00) + (value & 0xF00) + (C << 8) + (result & 0xFF);

				if (result > 0x9FF) {
					result += 0x600;
				}
				C = (result > 0x0FFF);
				result = (A & 0xF000) + (value & 0xF000) + (C << 12) + (result & 0xFFF);

				prevA = A;

				if (~(prevA ^ value) & (prevA ^ result) & 0x8000) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}

				if (result > 0x9FFF) {
					result += 0x6000;
				}

				A = result;
			}
			else {
				prevA = (int16_t)A;
				value |= ReadMemory(add + 1, true) << 8;

				A = (int16_t)A + (int16_t)value + C;

				if (~(prevA ^ value) & (prevA ^ A) & 0x8000) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}
			}
		}

		CheckNFlag(A, true, false);

		CheckZFlag(A, true, false);
		CheckCFlag(A, prevA, false);

		return !(P & MFlag) ? 8 : 7;
	}
		// STZ dp, X
	case 0x74:

		break;

		// ADC dp, X
	case 0x75: {

		uint16_t prevA;

		uint16_t add = ReadMemory(PC++, false);

		add |= DP << 8;
		add += X;

		int16_t value = ReadMemory(add, true);

		int8_t C = (P & CFlag) ? 1 : 0;

		int c = 0;

		if (DP & 0xFF) {
			c++;
		}

		if (P & MFlag) {
			uint16_t result;
			// Decimal Mode
			if (P & DFlag) {
				result = (A & 0x0F) + (uint8_t)(value & 0x0F) + C;
				if (result > 0x09) {
					result += 0x06;
				}
				C = (result > 0x0F);
				result = (A & 0xF0) + (uint8_t)(value & 0xF0) + (C << 4) + (result & 0x0F);

				prevA = A;

				if (~((prevA & 0xFF) ^ value) & ((prevA & 0xFF) ^ result) & 0x80) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}

				if (result > 0x9F) {
					result += 0x60;
				}

				A &= 0xFF00;
				A |= (uint8_t)result;
			}
			else {
				prevA = A;

				result = (A & 0xFF) + (uint8_t)value + C;

				A &= 0xFF00;
				A |= (uint8_t)result;

				if (~((prevA & 0xFF) ^ value) & ((prevA & 0xFF) ^ result) & 0x80) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}
			}
		}
		else {
			if (P & DFlag) {
				uint16_t result;
				value |= ReadMemory(add + 1, true) << 8;

				result = (A & 0x0F) + (value & 0x0F) + C;
				if (result > 0x09) {
					result += 0x06;
				}
				C = (result > 0x0F);
				result = (A & 0xF0) + (value & 0xF0) + (C << 4) + (result & 0x0F);

				if (result > 0x9F) {
					result += 0x60;
				}
				C = (result > 0x0FF);
				result = (A & 0xF00) + (value & 0xF00) + (C << 8) + (result & 0xFF);

				if (result > 0x9FF) {
					result += 0x600;
				}
				C = (result > 0x0FFF);
				result = (A & 0xF000) + (value & 0xF000) + (C << 12) + (result & 0xFFF);

				prevA = A;

				if (~(prevA ^ value) & (prevA ^ result) & 0x8000) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}

				if (result > 0x9FFF) {
					result += 0x6000;
				}

				A = result;
			}
			else {
				prevA = (int16_t)A;
				value |= ReadMemory(add + 1, false) << 8;

				A = (int16_t)A + (int16_t)value + C;

				if (~(prevA ^ value) & (prevA ^ A) & 0x8000) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}
			}
		}

		CheckNFlag(A, true, false);

		CheckZFlag(A, true, false);
		CheckCFlag(A, prevA, false);

		return 4 + c;
	}

		break;

		// ROR dp, X
	case 0x76:

		break;

		// ADC [dp], Y
	case 0x77: {

		uint16_t prevA;

		uint16_t indAdd = ReadMemory(PC++, false);
		indAdd |= DP << 8;

		int c = 0;
		if (DP & 0xFF) {
			c++;
		}

		uint32_t add = ReadMemory(indAdd, true);
		add |= ReadMemory(indAdd + 1, true) << 8;
		add |= ReadMemory(indAdd + 2, true) << 16;
		add += Y;

		int16_t value = ReadMemory(add, true);

		int8_t C = (P & CFlag) ? 1 : 0;

		if (P & MFlag) {
			uint16_t result;
			// Decimal Mode
			if (P & DFlag) {
				result = (A & 0x0F) + (uint8_t)(value & 0x0F) + C;
				if (result > 0x09) {
					result += 0x06;
				}
				int c = (result > 0x0F);
				result = (A & 0xF0) + (uint8_t)(value & 0xF0) + (c << 4) + (result & 0x0F);

				prevA = A;

				if (~((prevA & 0xFF) ^ value) & ((prevA & 0xFF) ^ result) & 0x80) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}

				if (result > 0x9F) {
					result += 0x60;
				}

				A &= 0xFF00;
				A |= (uint8_t)result;
			}
			else {
				prevA = A;

				result = (A & 0xFF) + (uint8_t)value + C;

				A &= 0xFF00;
				A |= (uint8_t)result;

				if (~((prevA & 0xFF) ^ value) & ((prevA & 0xFF) ^ result) & 0x80) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}
			}
		}
		else {
			c++;
			if (P & DFlag) {
				uint16_t result;
				value |= ReadMemory(add + 1, true) << 8;

				result = (A & 0x0F) + (value & 0x0F) + C;
				if (result > 0x09) {
					result += 0x06;
				}
				int c = (result > 0x0F);
				result = (A & 0xF0) + (value & 0xF0) + (c << 4) + (result & 0x0F);

				if (result > 0x9F) {
					result += 0x60;
				}
				c = (result > 0x0FF);
				result = (A & 0xF00) + (value & 0xF00) + (c << 8) + (result & 0xFF);

				if (result > 0x9FF) {
					result += 0x600;
				}
				c = (result > 0x0FFF);
				result = (A & 0xF000) + (value & 0xF000) + (c << 12) + (result & 0xFFF);

				prevA = A;

				if (~(prevA ^ value) & (prevA ^ result) & 0x8000) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}

				if (result > 0x9FFF) {
					result += 0x6000;
				}

				A = result;
			}
			else {
				prevA = (int16_t)A;
				value |= ReadMemory(add + 1, false) << 8;

				A = (int16_t)A + (int16_t)value + C;

				if (~(prevA ^ value) & (prevA ^ A) & 0x8000) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}
			}
		}

		CheckNFlag(A, true, false);

		CheckZFlag(A, true, false);
		CheckCFlag(A, prevA, false);

		return 6 + c;
	}
		// SEI
	case 0x78:

		P |= IFlag;

		return 2;

		// ADC addr, Y
	case 0x79: {

		uint16_t prevA;

		uint16_t add = ReadMemory(PC++, false);
		add |= ReadMemory(PC++, false) << 8;

		int c = 0;
		if ((P & XFlag) || ((add & 0xFF00) != ((add + X) & 0xFF00))) {
			c++;
		}
		add += Y;

		int16_t value = ReadMemory(add, true);

		int8_t C = (P & CFlag) ? 1 : 0;

		if (P & MFlag) {
			uint16_t result;
			// Decimal Mode
			if (P & DFlag) {
				result = (A & 0x0F) + (uint8_t)(value & 0x0F) + C;
				if (result > 0x09) {
					result += 0x06;
				}
				C = (result > 0x0F);
				result = (A & 0xF0) + (uint8_t)(value & 0xF0) + (C << 4) + (result & 0x0F);

				prevA = A;

				if (~((prevA & 0xFF) ^ value) & ((prevA & 0xFF) ^ result) & 0x80) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}

				if (result > 0x9F) {
					result += 0x60;
				}

				A &= 0xFF00;
				A |= (uint8_t)result;
			}
			else {
				prevA = A;

				result = (A & 0xFF) + (uint8_t)value + C;

				A &= 0xFF00;
				A |= (uint8_t)result;

				if (~((prevA & 0xFF) ^ value) & ((prevA & 0xFF) ^ result) & 0x80) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}
			}
		}
		else {
			c++;
			if (P & DFlag) {
				uint16_t result;
				value |= ReadMemory(add + 1, true) << 8;

				result = (A & 0x0F) + (value & 0x0F) + C;
				if (result > 0x09) {
					result += 0x06;
				}
				C = (result > 0x0F);
				result = (A & 0xF0) + (value & 0xF0) + (C << 4) + (result & 0x0F);

				if (result > 0x9F) {
					result += 0x60;
				}
				C = (result > 0x0FF);
				result = (A & 0xF00) + (value & 0xF00) + (C << 8) + (result & 0xFF);

				if (result > 0x9FF) {
					result += 0x600;
				}
				C = (result > 0x0FFF);
				result = (A & 0xF000) + (value & 0xF000) + (C << 12) + (result & 0xFFF);

				prevA = A;

				if (~(prevA ^ value) & (prevA ^ result) & 0x8000) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}

				if (result > 0x9FFF) {
					result += 0x6000;
				}

				A = result;
			}
			else {
				prevA = (int16_t)A;
				value |= ReadMemory(add + 1, false) << 8;

				A = (int16_t)A + (int16_t)value + C;

				if (~(prevA ^ value) & (prevA ^ A) & 0x8000) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}
			}
		}

		CheckNFlag(A, true, false);

		CheckZFlag(A, true, false);
		CheckCFlag(A, prevA, false);

		return 4 + c;
	}

		break;

		// PLY
	case 0x7A:

		Y &= 0xFF00;
		Y |= Pull();

		if (!(P & XFlag)) {
			Y &= 0x00FF;
			Y |= Pull() << 8;

			if (Y == 0) {
				P |= ZFlag;
			}
			else {
				P &= ~ZFlag;
			}

			if (Y & 0x8000) {
				P |= NFlag;
			}
			else {
				P &= ~NFlag;
			}
		}
		else {
			if ((Y & 0xFF) == 0) {
				P |= ZFlag;
			}
			else {
				P &= ~ZFlag;
			}

			if (Y & 0x80) {
				P |= NFlag;
			}
			else {
				P &= ~NFlag;
			}
		}

		return !(P & XFlag) ? 5 : 4;

		// TDC
	case 0x7B:

		break;

		// JMP (addr, X)
	case 0x7C:

		break;

		// ADC addr, X
	case 0x7D: {

		uint16_t prevA;

		uint16_t add = ReadMemory(PC++, false);
		add |= ReadMemory(PC++, false) << 8;

		int c = 0;
		if ((P & XFlag) || ((add & 0xFF00) != ((add + X) & 0xFF00))) {
			c++;
		}
		add += X;

		int16_t value = ReadMemory(add, true);

		int8_t C = (P & CFlag) ? 1 : 0;

		if (P & MFlag) {
			uint16_t result;
			// Decimal Mode
			if (P & DFlag) {
				result = (A & 0x0F) + (uint8_t)(value & 0x0F) + C;
				if (result > 0x09) {
					result += 0x06;
				}
				C = (result > 0x0F);
				result = (A & 0xF0) + (uint8_t)(value & 0xF0) + (C << 4) + (result & 0x0F);

				prevA = A;

				if (~((prevA & 0xFF) ^ value) & ((prevA & 0xFF) ^ result) & 0x80) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}

				if (result > 0x9F) {
					result += 0x60;
				}

				A &= 0xFF00;
				A |= (uint8_t)result;
			}
			else {
				prevA = A;

				result = (A & 0xFF) + (uint8_t)value + C;

				A &= 0xFF00;
				A |= (uint8_t)result;

				if (~((prevA & 0xFF) ^ value) & ((prevA & 0xFF) ^ result) & 0x80) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}
			}
		}
		else {
			c++;
			if (P & DFlag) {
				uint16_t result;
				value |= ReadMemory(add + 1, true) << 8;

				result = (A & 0x0F) + (value & 0x0F) + C;
				if (result > 0x09) {
					result += 0x06;
				}
				C = (result > 0x0F);
				result = (A & 0xF0) + (value & 0xF0) + (C << 4) + (result & 0x0F);

				if (result > 0x9F) {
					result += 0x60;
				}
				C = (result > 0x0FF);
				result = (A & 0xF00) + (value & 0xF00) + (C << 8) + (result & 0xFF);

				if (result > 0x9FF) {
					result += 0x600;
				}
				C = (result > 0x0FFF);
				result = (A & 0xF000) + (value & 0xF000) + (C << 12) + (result & 0xFFF);

				prevA = A;

				if (~(prevA ^ value) & (prevA ^ result) & 0x8000) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}

				if (result > 0x9FFF) {
					result += 0x6000;
				}

				A = result;
			}
			else {
				prevA = (int16_t)A;
				value |= ReadMemory(add + 1, false) << 8;

				A = (int16_t)A + (int16_t)value + C;

				if (~(prevA ^ value) & (prevA ^ A) & 0x8000) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}
			}
		}

		CheckNFlag(A, true, false);

		CheckZFlag(A, true, false);
		CheckCFlag(A, prevA, false);

		return 4 + c;
	}
		// ROR addr, X
	case 0x7E:

		break;

		// ADC long, X
	case 0x7F: {

		uint16_t prevA;

		uint32_t add = ReadMemory(PC++, false);
		add |= ReadMemory(PC++, false) << 8;
		add |= ReadMemory(PC++, false) << 16;

		int c = 0;

		add += X;

		int16_t value = ReadMemory(add, true);

		int8_t C = (P & CFlag) ? 1 : 0;

		if (P & MFlag) {
			uint16_t result;
			// Decimal Mode
			if (P & DFlag) {
				result = (A & 0x0F) + (uint8_t)(value & 0x0F) + C;
				if (result > 0x09) {
					result += 0x06;
				}
				C = (result > 0x0F);
				result = (A & 0xF0) + (uint8_t)(value & 0xF0) + (C << 4) + (result & 0x0F);

				prevA = A;

				if (~((prevA & 0xFF) ^ value) & ((prevA & 0xFF) ^ result) & 0x80) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}

				if (result > 0x9F) {
					result += 0x60;
				}

				A &= 0xFF00;
				A |= (uint8_t)result;
			}
			else {
				prevA = A;

				result = (A & 0xFF) + (uint8_t)value + C;

				A &= 0xFF00;
				A |= (uint8_t)result;

				if (~((prevA & 0xFF) ^ value) & ((prevA & 0xFF) ^ result) & 0x80) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}
			}
		}
		else {
			c++;
			if (P & DFlag) {
				uint16_t result;
				value |= ReadMemory(add + 1, true) << 8;

				result = (A & 0x0F) + (value & 0x0F) + C;
				if (result > 0x09) {
					result += 0x06;
				}
				C = (result > 0x0F);
				result = (A & 0xF0) + (value & 0xF0) + (C << 4) + (result & 0x0F);

				if (result > 0x9F) {
					result += 0x60;
				}
				C = (result > 0x0FF);
				result = (A & 0xF00) + (value & 0xF00) + (C << 8) + (result & 0xFF);

				if (result > 0x9FF) {
					result += 0x600;
				}
				C = (result > 0x0FFF);
				result = (A & 0xF000) + (value & 0xF000) + (C << 12) + (result & 0xFFF);

				prevA = A;

				if (~(prevA ^ value) & (prevA ^ result) & 0x8000) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}

				if (result > 0x9FFF) {
					result += 0x6000;
				}

				A = result;
			}
			else {
				prevA = (int16_t)A;
				value |= ReadMemory(add + 1, false) << 8;

				A = (int16_t)A + (int16_t)value + C;

				if (~(prevA ^ value) & (prevA ^ A) & 0x8000) {
					P |= VFlag;
				}
				else {
					P &= ~VFlag;
				}
			}
		}

		CheckNFlag(A, true, false);

		CheckZFlag(A, true, false);
		CheckCFlag(A, prevA, false);

		return 5 + c;
	}
		// BRA nearlabel
	case 0x80: {

		int8_t offset = ReadMemory(PC++, false);

		int c = 0;

		uint16_t page = PC & 0xFF00;
		PC += offset;
		if ((PC & 0xFF00) != page && emulationMode) {
			c = 1;
		}

		return 3 + c;
	}
		// STA (dp, X)
	case 0x81:

		break;

		// BRL label
	case 0x82:

		break;

		// STA sr, S
	case 0x83:

		break;

		// STY dp
	case 0x84:

		break;

		// STA dp
	case 0x85: {

		uint32_t add = ReadMemory(PC++, false);

		add |= DP << 8;

		WriteMemory(add, A & 0xFF, true);

		int c = 0;

		if (!(P & MFlag)) {
			WriteMemory(add + 1, A >> 8, true);
			c++;
		}

		if ((DP & 0xFF) != 0) {
			c++;
		}

		return 3 + c;
	}
		// STX dp
	case 0x86: {

		uint32_t add = ReadMemory(PC++, false);

		add |= DP << 8;

		WriteMemory(add, X & 0xFF, true);

		int c = 0;

		if (!(P & XFlag)) {
			WriteMemory(add + 1, X >> 8, true);
			c++;
		}

		if ((DP & 0xFF) != 0) {
			c++;
		}

		return 3 + c;
	}
		// STA [dp]
	case 0x87:

		break;

		// DEY
	case 0x88:

		Y--;

		CheckNFlag(Y, false, true);
		CheckZFlag(Y, false, true);

		return 2;

		// BIT #const
	case 0x89:

		break;
		
		// TXA
	case 0x8A:

		break;

		// PHB
	case 0x8B:

		break;

		// STY addr
	case 0x8C: {

		uint16_t add = ReadMemory(PC++, false);
		add |= ReadMemory(PC++, false) << 8;

		WriteMemory(add, Y & 0xFF, false);

		if (!(P & XFlag)) {
			WriteMemory(add + 1, (Y & 0xFF00) >> 8, false);
		}

		return !(P & XFlag) ? 5 : 4;
	}
		// STA addr
	case 0x8D: {
		uint16_t add = ReadMemory(PC++, false);
		add |= ReadMemory(PC++, false) << 8;

		WriteMemory(add, A & 0xFF, false);

		if (!(P & MFlag)) {
			WriteMemory(add + 1, (A & 0xFF00) >> 8, false);
		}

		return !(P & MFlag) ? 5 : 4;
	}
		// STX addr
	case 0x8E: {

		uint16_t add = ReadMemory(PC++, false);
		add |= ReadMemory(PC++, false) << 8;

		WriteMemory(add, X & 0xFF, false);

		if (!(P & XFlag)) {
			WriteMemory(add + 1, (X & 0xFF00) >> 8, false);
		}

		return !(P & XFlag) ? 5 : 4;
	}
		// STA long
	case 0x8F:

		break;

		// BCC nearlabel
	case 0x90:

		break;

		// STA (dp), Y
	case 0x91:

		break;

		// STA (dp)
	case 0x92:

		break;

		// STA (sr, S), Y
	case 0x93:

		break;

		// STY dp, X
	case 0x94:

		break;

		// STA dp, X
	case 0x95:

		break;

		// STX dp, Y
	case 0x96:

		break;

		// STA [dp], Y
	case 0x97:

		break;

		// TYA
	case 0x98:

		break;

		// STA addr, Y
	case 0x99:

		break;

		// TXS
	case 0x9A:

		SP = X;

		return 2;

		// TXY
	case 0x9B:

		break;

		// STZ addr
	case 0x9C: {

		uint16_t dir = ReadMemory(PC++, false);
		dir |= ReadMemory(PC++, false) << 8;

		WriteMemory(dir, 0, false);

		if (!(P & MFlag)) {
			WriteMemory(dir + 1, 0, false);
		}

		return !(P & MFlag) ? 5 : 4;
	}
		// STA addr, X
	case 0x9D:

		break;

		// STZ addr, X
	case 0x9E:

		break;

		// STA long, X
	case 0x9F:

		break;

		// LDY #const
	case 0xA0:

		Y = ReadMemory(PC++, false);

		if (!(P & XFlag)) {
			Y |= ReadMemory(PC++, false) << 8;
		}

		CheckNFlag(Y, false, true);
		CheckZFlag(Y, false, true);

		return !(P & XFlag) ? 3 : 2;

		// LDA (dp, X)
	case 0xA1:

		break;

		// LDX #const
	case 0xA2:

		X = ReadMemory(PC++, false);

		if (!(P & XFlag)) {
			X |= ReadMemory(PC++, false) << 8;
		}

		CheckNFlag(X, false, true);
		CheckZFlag(X, false, true);

		return !(P & XFlag) ? 3 : 2;

		// LDA sr, S
	case 0xA3:

		break;

		// LDY dp
	case 0xA4:

		break;

		// LDA dp
	case 0xA5: {
		uint32_t add = ReadMemory(PC++, false);

		add |= DP << 8;

		A &= 0xFF00;
		A |= ReadMemory(add, true);

		int c = 0;

		if (!(P & MFlag)) {
			A |= ReadMemory(add + 1, true) << 8;
			c++;

			if (A == 0) {
				P |= ZFlag;
			}
			else {
				P &= ~ZFlag;
			}

			if (A & 0x8000) {
				P |= NFlag;
			}
			else {
				P &= ~NFlag;
			}
		}
		else {
			if ((A & 0xFF) == 0) {
				P |= ZFlag;
			}
			else {
				P &= ~ZFlag;
			}

			if (A & 0x80) {
				P |= NFlag;
			}
			else {
				P &= ~NFlag;
			}
		}

		if ((DP & 0xFF) != 0) {
			c++;
		}

		return 3 + c;
	}
		// LDX dp
	case 0xA6: {

		uint8_t add = ReadMemory(PC++, false);

		uint16_t value = ReadMemory(add | (DP << 8), true);

		int c = 0;

		if (!(P & XFlag)) {
			value |= ReadMemory((add | (DP << 8)) + 1, true) << 8;
			c++;
		}

		X = value;

		if ((DP & 0xFF) != 0) {
			c++;
		}

		CheckNFlag(X, false, true);
		CheckZFlag(X, false, true);

		return 3 + c;
	}
		// LDA [dp]
	case 0xA7: {

		uint16_t indAdd = ReadMemory(PC++, false);
		uint32_t add = ReadMemory((DP << 8) | indAdd, false);
		add |= ReadMemory(((DP << 8) | indAdd) + 1, false) << 8;
		add |= ReadMemory(((DP << 8) | indAdd) + 2, false) << 16;

		uint16_t value = ReadMemory(add, true);

		A &= 0xFF00;
		A |= value;

		int c = 0;
		if (!(P & MFlag)) {
			c++;
			A &= 0x00FF;
			value |= ReadMemory(add + 1, true) << 8;
		}

		if ((DP & 0xFF) != 0) {
			c++;
		}

		CheckNFlag(A, true, false);
		CheckZFlag(A, true, false);

		return 6 + c;
	}
		// TAY
	case 0xA8:

		break;

		// LDA #const
	case 0xA9:

		A &= 0xFF00;
		A |= ReadMemory(PC++, false);

		if (!(P & MFlag)) {
			A &= 0x00FF;
			A |= ReadMemory(PC++, false) << 8;
		}

		CheckNFlag(A, true, false);
		CheckZFlag(A, true, false);

		return !(P & MFlag) ? 3 : 2;

		// TAX
	case 0xAA:

		break;

		// PLB
	case 0xAB:

		PB = Pull();

		CheckNFlag(PB, false, false);
		CheckZFlag(PB, false, false);

		return 4;

		// LDY addr
	case 0xAC:

		break;

		// LDA addr
	case 0xAD:

		break;

		// LDX addr
	case 0xAE:

		break;

		// LDA long
	case 0xAF:

		break;

		// BCS nearlabel
	case 0xB0:

		break;

		// LDA (dp), Y
	case 0xB1:

		break;

		// LDA (dp)
	case 0xB2:

		break;

		// LDA (sr, S), Y
	case 0xB3:

		break;

		// LDY dp, X
	case 0xB4:

		break;

		// LDA dp, X
	case 0xB5:

		break;

		// LDX dp, Y
	case 0xB6:

		break;

		// LDA [dp], Y
	case 0xB7:

		break;

		// CLV
	case 0xB8:

		P &= ~VFlag;

		return 2;

		// LDA addr, Y
	case 0xB9:

		break;

		// TSX
	case 0xBA:

		break;

		// TYX
	case 0xBB:

		break;

		// LDY addr, X
	case 0xBC:

		break;

		// LDA addr, X
	case 0xBD: {

		uint16_t add = ReadMemory(PC++, false);
		add |= ReadMemory(PC++, false) << 8;

		int c = 0;

		if (!(P & XFlag)) {
			c++;

			add += X;
		}
		else {
			uint8_t page = (add & 0xFF00) >> 8;
			add += X & 0xFF;

			if (((add & 0xFF00) >> 8) != page) {
				c++;
			}
		}

		A &= 0xFF00;
		A |= ReadMemory(add, false);

		if (!(P & MFlag)) {
			c++;
			A &= 0x00FF;
			A |= ReadMemory(add + 1, false) << 8;
		}

		CheckNFlag(A, true, false);
		CheckZFlag(A, true, false);

		return 4 + c;
	}
		// LDX addr, Y
	case 0xBE:

		break;

		// LDA long, X
	case 0xBF:

		break;

		// CPY #const
	case 0xC0:

		break;

		// CMP (dp, X)
	case 0xC1:

		break;

		// REP #const
	case 0xC2: {

		uint8_t value = ReadMemory(PC++, false);
		P &= ~value;

		if (P & XFlag) {
			X &= 0xFF;
			Y &= 0xFF;
		}

		return 3;
	}

		// CMP sr, S
	case 0xC3:

		break;

		// CPY dp
	case 0xC4:

		break;

		// CMP dp
	case 0xC5:

		break;

		// DEC dp
	case 0xC6:

		break;

		// CMP [dp]
	case 0xC7:

		break;

		// INY
	case 0xC8:

		Y++;

		CheckNFlag(Y, false, true);
		CheckZFlag(Y, false, true);

		return 2;

		// CMP #const
	case 0xC9: {

		uint16_t value = ReadMemory(PC++, false);

		if (!(P & MFlag)) {
			value |= ReadMemory(PC++, false) << 8;

			if (value == A) {
				P |= ZFlag;
			}
			else {
				P &= ~ZFlag;
			}

			if (A < value) {
				P &= ~CFlag;
			}
			else {
				P |= CFlag;
			}

			value = A - value;

		}
		else {
			if (value == (A & 0xFF)) {
				P |= ZFlag;
			}
			else {
				P &= ~ZFlag;
			}

			if ((A & 0xFF) < value) {
				P &= ~CFlag;
			}
			else {
				P |= CFlag;
			}

			value = (A & 0xFF) - value;
		}

		CheckNFlag(value, true, false);

		return !(P & MFlag) ? 3 : 2;
	}
		// DEX
	case 0xCA:

		X--;

		CheckNFlag(X, false, true);
		CheckZFlag(X, false, true);

		return 2;

		// WAI
	case 0xCB:

		break;

		// CPY addr
	case 0xCC:

		break;

		// CMP addr
	case 0xCD: {

		uint16_t dir = ReadMemory(PC++, false);
		dir |= ReadMemory(PC++, false) << 8;

		uint16_t value = ReadMemory(dir, false);

		if (!(P & MFlag)) {
			value |= ReadMemory(dir + 1, false) << 8;

			if (value == A) {
				P |= ZFlag;
			}
			else {
				P &= ~ZFlag;
			}

			if (A < value) {
				P &= ~CFlag;
			}
			else {
				P |= CFlag;
			}
		}
		else {
			if (value == (A & 0xFF)) {
				P |= ZFlag;
			}
			else {
				P &= ~ZFlag;
			}

			if ((A & 0xFF) < value) {
				P &= ~CFlag;
			}
			else {
				P |= CFlag;
			}
		}

		value = A - value;

		CheckNFlag(value, true, false);

		return !(P & MFlag) ? 5 : 4;
	}
		// DEC addr
	case 0xCE:

		break;

		// CMP long
	case 0xCF:

		break;

		// BNE nearlabel
	case 0xD0: {

		int8_t offset = ReadMemory(PC++, false);

		if (!(P & ZFlag)) {

			int c = 0;

			uint16_t page = PC & 0xFF00;
			PC += offset;
			if ((PC & 0xFF00) != page && emulationMode) {
				c = 1;
			}

			return 3 + c;

		}

		return 2;
	}

		// CMP (dp), Y
	case 0xD1:

		break;

		// CMP (dp)
	case 0xD2:

		break;

		// CMP (sr, S), Y
	case 0xD3:

		break;

		// PEI (dp)
	case 0xD4:

		break;

		// CMP dp, X
	case 0xD5:

		break;

		// DEC dp, X
	case 0xD6:

		break;

		// CMP [dp], Y
	case 0xD7:

		break;

		// CLD
	case 0xD8:

		P &= ~DFlag;

		return 2;

		// CMP addr, Y
	case 0xD9:

		break;

		// PHX
	case 0xDA:

		if (!(P & XFlag)) {
			Push((X & 0xFF00) >> 8);
		}
		Push(X & 0xFF);

		return !(P & XFlag) ? 4 : 3;

		// STP
	case 0xDB:

		break;

		// JMP [addr]
	case 0xDC:

		break;

		// CMP addr, X
	case 0xDD:

		break;

		// DEC addr, X
	case 0xDE:

		break;

		// CMP long, X
	case 0xDF:

		break;

		// CPX #const
	case 0xE0: {

		uint16_t value = ReadMemory(PC++, false);

		if (!(P & XFlag)) {
			value |= ReadMemory(PC++, false) << 8;

		}

		if (value == X) {
			P |= ZFlag;
		}
		else {
			P &= ~ZFlag;
		}

		if (X < value) {
			P &= ~CFlag;
		}
		else {
			P |= CFlag;
		}

		value = X - value;

		CheckNFlag(value, false, true);

		return !(P & XFlag) ? 3 : 2;
	}
		// SBC (dp, X)
	case 0xE1:

		break;

		// SEP #const
	case 0xE2: {

		uint8_t value = ReadMemory(PC++, false);
		P |= value;

		return 3;
	}

		// SBC sr, S
	case 0xE3:

		break;

		// CPX dp
	case 0xE4:

		break;

		// SBC dp
	case 0xE5:

		break;

		// INC dp
	case 0xE6:

		break;

		// SBC [dp]
	case 0xE7:

		break;

		// INX
	case 0xE8:

		X++;

		CheckNFlag(X, false, true);
		CheckZFlag(X, false, true);

		return 2;

		// SBC #const
	case 0xE9:

		break;

		// NOP
	case 0xEA:

		break;

		// XBA
	case 0xEB: {

		uint16_t low = A & 0xFF;
		uint8_t high = (A & 0xFF00) >> 8;

		A = high | (low << 8);

		CheckNFlag(A, true, false);
		CheckZFlag(A, true, false);

		return 3;
	}

		// CPX addr
	case 0xEC: {

		uint16_t dir = ReadMemory(PC++, false);
		dir |= ReadMemory(PC++, false) << 8;

		uint16_t value = ReadMemory(dir, false);

		if (!(P & XFlag)) {
			value |= ReadMemory(dir + 1, false) << 8;
		}

		if (value == X) {
			P |= ZFlag;
		}
		else {
			P &= ~ZFlag;
		}

		if (X < value) {
			P &= ~CFlag;
		}
		else {
			P |= CFlag;
		}

		value = X - value;

		CheckNFlag(value, false, true);

		return !(P & XFlag) ? 5 : 4;
	}
		// SBC addr
	case 0xED:

		break;

		// INC addr
	case 0xEE:

		break;

		// SBC long
	case 0xEF:

		break;

		// BEQ nearlabel
	case 0xF0: {

		int8_t offset = ReadMemory(PC++, false);

		if (P & ZFlag) {

			int c = 0;

			uint16_t page = PC & 0xFF00;
			PC += offset;
			if ((PC & 0xFF00) != page && emulationMode) {
				c = 1;
			}

			return 3 + c;

		}

		return 2;
	}
		// SBC (dp), Y
	case 0xF1:

		break;

		// SBC (dp)
	case 0xF2:

		break;

		// SBC (sr, S), Y
	case 0xF3:

		break;

		// PEA addr
	case 0xF4:

		break;

		// SBC dp, X
	case 0xF5:

		break;

		// INC dp, X
	case 0xF6:

		break;

		// SBC [dp], Y
	case 0xF7:

		break;

		// SED
	case 0xF8:

		P |= DFlag;

		return 2;

		// SBC addr, Y
	case 0xF9:

		break;

		// PLX
	case 0xFA:

		X &= 0xFF00;
		X |= Pull();

		if (!(P & XFlag)) {
			X &= 0x00FF;
			X |= Pull() << 8;

			if (X == 0) {
				P |= ZFlag;
			}
			else {
				P &= ~ZFlag;
			}

			if (X & 0x8000) {
				P |= NFlag;
			}
			else {
				P &= ~NFlag;
			}
		}
		else {
			if ((X & 0xFF) == 0) {
				P |= ZFlag;
			}
			else {
				P &= ~ZFlag;
			}

			if (X & 0x80) {
				P |= NFlag;
			}
			else {
				P &= ~NFlag;
			}
		}

		return !(P & XFlag) ? 5 : 4;

		// XCE
	case 0xFB:

		emulationMode = false;

		P |= CFlag;

		return 2;

		// JSR (addr, X)
	case 0xFC:

		break;

		// SBC addr, X
	case 0xFD:

		break;

		// INC addr, X
	case 0xFE:

		break;

		// SBC long, X
	case 0xFF:

		break;

	}

	return 2;
}

void CPURicoh::CheckNFlag(uint16_t value, bool isA, bool isX) {

	if ((isA && (P & MFlag)) || (isX && (P & XFlag))) {
		if ((value & 0x80) == 0x80) {
			P |= NFlag;
		}
		else {
			P &= ~NFlag;
		}
	}
	else {
		if ((value & 0x8000) == 0x8000) {
			P |= NFlag;
		}
		else {
			P &= ~NFlag;
		}
	}	
}

void CPURicoh::CheckVFlag(uint16_t value, uint16_t prevValue, uint16_t operant, bool isSub) {
	if (isSub) {

		// 8-Bit
		if (P & MFlag) {
			if (((prevValue & 0x80) && !(operant & 0x80) && (value & 0x80)) || (!(prevValue & 0x80) && (operant & 0x80) && !(value & 0x80))) {
				P |= VFlag;
			}
			else {
				P &= ~VFlag;
			}
		}
		// 16-Bit
		else {
			if (((prevValue & 0x8000) && !(operant & 0x8000) && (value & 0x8000)) || (!(prevValue & 0x8000) && (operant & 0x8000) && !(value & 0x8000))) {
				P |= VFlag;
			}
			else {
				P &= ~VFlag;
			}
		}
	}
	else {
		// 8-Bit
		if (P & MFlag) {
			if (((prevValue & 0x80) && (operant & 0x80) && !(value & 0x80)) || (!(prevValue & 0x80) && !(operant & 0x80) && (value & 0x80))) {
				P |= VFlag;
			}
			else {
				P &= ~VFlag;
			}
		}
		// 16-Bit
		else {
			if (((prevValue & 0x8000) && (operant & 0x8000) && !(value & 0x8000)) || (!(prevValue & 0x8000) && !(operant & 0x8000) && (value & 0x8000))) {
				P |= VFlag;
			}
			else {
				P &= ~VFlag;
			}
		}
	}
}

void CPURicoh::CheckZFlag(uint16_t value, bool isA, bool isX) {

	if ((isA && (P & MFlag)) || (isX && (P & XFlag))) {
		if ((value & 0xFF) == 0) {
			P |= ZFlag;
		}
		else {
			P &= ~ZFlag;
		}
	}
	else {
		if (value == 0) {
			P |= ZFlag;
		}
		else {
			P &= ~ZFlag;
		}
	}
}

void CPURicoh::CheckCFlag(uint16_t value, uint16_t prevValue, bool isSub) {

	if (isSub) {
		if (prevValue < value) {
			P |= CFlag;
		}
		else {
			P &= ~CFlag;
		}
	}
	else {
		if (prevValue > value) {
			P |= CFlag;
		}
		else {
			P &= ~CFlag;
		}
	}
}
