#include "APU.h"

#include "Memory.h"

APU::APU()
{
	PC = 0xFFC0;

	A = 0;
	X = 0;
	Y = 0;

	mem = nullptr;

	for (int i = 0; i < 64; i++)
	{
		ARAM[0xFFC0 - 0x200 + i] = bootRom[i];
	}
}

bool APUdebug = false;

int APU::Clock()
{
	int opCycles = 0;

	if (cycles == 0) {
		opcode = ReadAPUMem(PC++);

		if (APUdebug) {
			Debug();
		}

		opCycles = Execute();
		cycles = opCycles;
	}

	cycles--;

	return opCycles;
}

void APU::Debug()
{
	std::cout << std::setfill('0');
	std::cout << std::hex << std::setw(4) << PC - 1 << " ";
	std::cout << std::setw(2) << (int)opcode << " ";
	std::cout << "A:" << std::setw(2) << (int)A << " ";
	std::cout << "X:" << std::setw(2) << (int)X << " ";
	std::cout << "Y:" << std::setw(2) << (int)Y << " ";
	std::cout << "S:" << std::setw(2) << (int)SP << " ";
	std::cout << "PSW:" << std::setw(2) << (int)PSW << " ";

	std::cout << "Port0:" << (int)CPUPorts[0] << ", " << (int)APUPorts[0] << " ";
	std::cout << "Port1:" << (int)CPUPorts[1] << ", " << (int)APUPorts[1] << " ";
	std::cout << "Port2:" << (int)CPUPorts[2] << ", " << (int)APUPorts[2] << " ";
	std::cout << "Port3:" << (int)CPUPorts[3] << ", " << (int)APUPorts[3];

	std::cout << "" << std::endl;
}

int APU::Execute()
{
	switch (opcode)
	{
	// NOP
	case 0x00: {
		return 2;
	}

	// TCALL 0
	case 0x01: {
		uint16_t newPC = ReadAPUMem(0xFFDE);
		newPC |= ReadAPUMem(0xFFDF) << 8;

		Push(PC >> 8);
		Push(PC & 0xFF);

		PC = newPC;

		return 8;
	}

	// SET1 d.0
	case 0x02: {
		std::cout << "NO" << std::endl;
		break;
	}

	// BBS d.0, r
	case 0x03: {
		std::cout << "NO" << std::endl;
		break;
	}

	// OR A, d
	case 0x04: {
		std::cout << "NO" << std::endl;
		break;
	}

	// OR A, !a
	case 0x05: {
		std::cout << "NO" << std::endl;
		break;
	}

	// OR A, (X)
	case 0x06: {
		std::cout << "NO" << std::endl;
		break;
	}

	// OR A, [d+X]
	case 0x07: {
		std::cout << "NO" << std::endl;
		break;
	}

	// OR A, #i
	case 0x08: {
		std::cout << "NO" << std::endl;
		break;
	}

	// OR dd, ds
	case 0x09: {
		std::cout << "NO" << std::endl;
		break;
	}

	// OR1 C, m.b
	case 0x0A: {
		std::cout << "NO" << std::endl;
		break;
	}

	// ASL d
	case 0x0B: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		uint8_t value = ReadAPUMem(add);

		PSW &= ~CFlag;
		if ((value & 0x80) == 0x80) {
			PSW |= CFlag;
		}

		value = value << 1;

		WriteAPUMem(add, value);

		CheckNFlag(value);
		CheckZFlag(value);

		return 4;
	}

	// ASL !a
	case 0x0C: {
		uint16_t add = ReadAPUMem(PC++);
		add |= ReadAPUMem(PC++) << 8;

		uint8_t value = ReadAPUMem(add);

		PSW &= ~CFlag;
		if ((value & 0x80) == 0x80) {
			PSW |= CFlag;
		}

		value = value << 1;

		WriteAPUMem(add, value);

		CheckNFlag(value);
		CheckZFlag(value);

		return 5;
	}

	// PUSH PSW
	case 0x0D: {
		Push(PSW);
		return 4;
	}

	// TSET1 !a
	case 0x0E: {
		std::cout << "NO" << std::endl;
		break;
	}

	// BRK
	case 0x0F: {
		PSW |= 0x10;
		PSW &= ~0x04;

		// There are no interrupts

		return 8;
	}

	// BPL r
	case 0x10: {
		int8_t offset = ReadAPUMem(PC++);

		if ((PSW & NFlag) != NFlag) {
			PC += offset;

			return 4;

		}

		return 2;
	}

	// TCALL 1
	case 0x11: {
		uint16_t newPC = ReadAPUMem(0xFFDC);
		newPC |= ReadAPUMem(0xFFDD) << 8;

		Push(PC >> 8);
		Push(PC & 0xFF);

		PC = newPC;

		return 8;
	}

	// CLR1 d.0
	case 0x12: {
		std::cout << "NO" << std::endl;
		break;
	}

	// BBC d.0, r
	case 0x13: {
		std::cout << "NO" << std::endl;
		break;
	}

	// OR A, d+X
	case 0x14: {
		std::cout << "NO" << std::endl;
		break;
	}

	// OR A, !a+X
	case 0x15: {
		std::cout << "NO" << std::endl;
		break;
	}

	// OR A, !a+Y
	case 0x16: {
		std::cout << "NO" << std::endl;
		break;
	}

	// OR A, [d]+Y
	case 0x17: {
		std::cout << "NO" << std::endl;
		break;
	}

	// OR d, #i
	case 0x18: {
		std::cout << "NO" << std::endl;
		break;
	}

	// OR (X), (Y)
	case 0x19: {
		std::cout << "NO" << std::endl;
		break;
	}

	// DECW d
	case 0x1A: {
		std::cout << "NO" << std::endl;
		break;
	}

	// ASL d+X
	case 0x1B: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		uint8_t value = ReadAPUMem(add + X);

		PSW &= ~CFlag;
		if ((value & 0x80) == 0x80) {
			PSW |= CFlag;
		}

		value = value << 1;

		WriteAPUMem(add + X, value);

		CheckNFlag(value);
		CheckZFlag(value);

		return 5;
	}

	// ASL A
	case 0x1C: {
		PSW &= ~CFlag;
		if ((A & 0x80) == 0x80) {
			PSW |= CFlag;
		}

		A = A << 1;

		CheckNFlag(A);
		CheckZFlag(A);

		return 2;
	}

	// DEC X
	case 0x1D: {
		X--;
		CheckNFlag(X);
		CheckZFlag(X);

		return 2;
	}

	// CMP X, !a
	case 0x1E: {
		uint16_t add = ReadAPUMem(PC++);
		add |= ReadAPUMem(PC++);

		uint8_t valueB = ReadAPUMem(add);

		CMP(X, valueB);

		return 4;
	}

	// JMP [!a+X]
	case 0x1F: {
		uint16_t add = ReadAPUMem(PC++);
		add |= ReadAPUMem(PC++) << 8;

		uint16_t newPC = ReadAPUMem(add + X);
		newPC |= ReadAPUMem(add + X + 1) << 8;
		std::cout << "JUMP! " << std::hex << (int)newPC << std::endl;

		PC = newPC;

		return 6;
	}

	// CLRP
	case 0x20: {
		PSW &= ~PFlag;
		return 2;
	}

	// TCALL 2
	case 0x21: {
		uint16_t newPC = ReadAPUMem(0xFFDA);
		newPC |= ReadAPUMem(0xFFDB) << 8;

		Push(PC >> 8);
		Push(PC & 0xFF);

		PC = newPC;

		return 8;
	}

	// SET1 d.1
	case 0x22: {
		std::cout << "NO" << std::endl;
		break;
	}

	// BBS d.1, r
	case 0x23: {
		std::cout << "NO" << std::endl;
		break;
	}

	// AND A, d
	case 0x24: {
		std::cout << "NO" << std::endl;
		break;
	}

	// AND A, !a
	case 0x25: {
		std::cout << "NO" << std::endl;
		break;
	}

	// AND A, (X)
	case 0x26: {
		std::cout << "NO" << std::endl;
		break;
	}

	// AND A, [d+X]
	case 0x27: {
		std::cout << "NO" << std::endl;
		break;
	}

	// AND A, #i
	case 0x28: {
		std::cout << "NO" << std::endl;
		break;
	}

	// AND dd, ds
	case 0x29: {
		std::cout << "NO" << std::endl;
		break;
	}

	// OR1 C, /m.b
	case 0x2A: {
		std::cout << "NO" << std::endl;
		break;
	}

	// ROL d
	case 0x2B: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		uint8_t value = ReadAPUMem(add);

		uint8_t C = (PSW & CFlag) ? 1 : 0;

		PSW &= ~CFlag;
		if ((A & 0x80) == 0x80) {
			PSW |= CFlag;
		}

		value = value << 1;

		if (C == 1) {
			value |= 0x01;
		}

		WriteAPUMem(add, value);

		CheckNFlag(value);
		CheckZFlag(value);

		return 4;
	}

	// ROL !a
	case 0x2C: {
		uint16_t add = ReadAPUMem(PC++);
		add |= ReadAPUMem(PC++) << 8;

		uint8_t value = ReadAPUMem(add);

		uint8_t C = (PSW & CFlag) ? 1 : 0;

		PSW &= ~CFlag;
		if ((A & 0x80) == 0x80) {
			PSW |= CFlag;
		}

		value = value << 1;

		if (C == 1) {
			value |= 0x01;
		}

		WriteAPUMem(add, value);

		CheckNFlag(value);
		CheckZFlag(value);

		return 5;
	}

	// PUSH A
	case 0x2D: {
		Push(A);
		return 4;
	}

	// CBNE d, r
	case 0x2E: {
		std::cout << "NO" << std::endl;
		break;
	}

	// BRA r
	case 0x2F: {
		int8_t offset = ReadAPUMem(PC++);

		PC += offset;

		return 4;
	}

	// BMI r
	case 0x30: {
		int8_t offset = ReadAPUMem(PC++);

		if ((PSW & NFlag) == NFlag) {
			PC += offset;

			return 4;

		}

		return 2;
	}

	// TCALL 3
	case 0x31: {
		uint16_t newPC = ReadAPUMem(0xFFD8);
		newPC |= ReadAPUMem(0xFFD9) << 8;

		Push(PC >> 8);
		Push(PC & 0xFF);

		PC = newPC;

		return 8;
	}

	// CLR1 d.1
	case 0x32: {
		std::cout << "NO" << std::endl;
		break;
	}

	// BBC d.1, r
	case 0x33: {
		std::cout << "NO" << std::endl;
		break;
	}

	// AND A, d+X
	case 0x34: {
		std::cout << "NO" << std::endl;
		break;
	}

	// AND A, !a+X
	case 0x35: {
		std::cout << "NO" << std::endl;
		break;
	}

	// AND A, !a+Y
	case 0x36: {
		std::cout << "NO" << std::endl;
		break;
	}

	// AND A, [d]+Y
	case 0x37: {
		std::cout << "NO" << std::endl;
		break;
	}

	// AND d, #i
	case 0x38: {
		std::cout << "NO" << std::endl;
		break;
	}

	// AND (X), (Y)
	case 0x39: {
		std::cout << "NO" << std::endl;
		break;
	}

	// INCW d
	case 0x3A: {
		std::cout << "NO" << std::endl;
		break;
	}

	// ROL d+X
	case 0x3B: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		uint8_t value = ReadAPUMem(add + X);

		uint8_t C = (PSW & CFlag) ? 1 : 0;

		PSW &= ~CFlag;
		if ((A & 0x80) == 0x80) {
			PSW |= CFlag;
		}

		value = value << 1;

		if (C == 1) {
			value |= 0x01;
		}

		WriteAPUMem(add + X, value);

		CheckNFlag(value);
		CheckZFlag(value);

		return 5;
	}

	// ROL A
	case 0x3C: {
		uint8_t C = (PSW & CFlag) ? 1 : 0;

		PSW &= ~CFlag;
		if ((A & 0x80) == 0x80) {
			PSW |= CFlag;
		}

		A = A << 1;

		if (C == 1) {
			A |= 0x01;
		}

		CheckNFlag(A);
		CheckZFlag(A);

		return 2;
	}

	// INC X
	case 0x3D: {
		X++;
		CheckNFlag(X);
		CheckZFlag(X);

		return 2;
	}

	// CMP X, d
	case 0x3E: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		uint8_t valueB = ReadAPUMem(add);

		CMP(X, valueB);

		return 3;
	}

	// CALL !a
	case 0x3F: {
		uint16_t newPC = ReadAPUMem(PC++);
		newPC |= ReadAPUMem(PC++) << 8;

		Push(PC >> 8);
		Push(PC & 0xFF);

		PC = newPC;

		return 8;
	}

	// SETP
	case 0x40: {
		PSW |= PFlag;
		return 2;
	}

	// TCALL 4
	case 0x41: {
		uint16_t newPC = ReadAPUMem(0xFFD6);
		newPC |= ReadAPUMem(0xFFD7) << 8;

		Push(PC >> 8);
		Push(PC & 0xFF);

		PC = newPC;

		return 8;
	}

	// SET1 d.2
	case 0x42: {
		std::cout << "NO" << std::endl;
		break;
	}

	// BBS d.2, r
	case 0x43: {
		std::cout << "NO" << std::endl;
		break;
	}

	// EOR A, d
	case 0x44: {
		std::cout << "NO" << std::endl;
		break;
	}

	// EOR A, !a
	case 0x45: {
		std::cout << "NO" << std::endl;
		break;
	}

	// EOR A, (X)
	case 0x46: {
		std::cout << "NO" << std::endl;
		break;
	}

	// EOR A, [d+X]
	case 0x47: {
		std::cout << "NO" << std::endl;
		break;
	}

	// EOR A, #i
	case 0x48: {
		std::cout << "NO" << std::endl;
		break;
	}

	// EOR dd, ds
	case 0x49: {
		std::cout << "NO" << std::endl;
		break;
	}

	// AND1 C, m.b
	case 0x4A: {
		std::cout << "NO" << std::endl;
		break;
	}

	// LSR d
	case 0x4B: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		uint8_t value = ReadAPUMem(add);

		PSW &= ~CFlag;
		if ((value & 0x01) == 0x01) {
			PSW |= CFlag;
		}

		value = value >> 1;

		WriteAPUMem(add, value);

		CheckNFlag(value);
		CheckZFlag(value);

		return 4;
	}

	// LSR !a
	case 0x4C: {
		uint16_t add = ReadAPUMem(PC++);
		add |= ReadAPUMem(PC++) << 8;

		uint8_t value = ReadAPUMem(add);

		PSW &= ~CFlag;
		if ((value & 0x01) == 0x01) {
			PSW |= CFlag;
		}

		value = value >> 1;

		WriteAPUMem(add, value);

		CheckNFlag(value);
		CheckZFlag(value);

		return 5;
	}

	// PUSH X
	case 0x4D: {
		Push(X);
		return 4;
	}

	// TCLR1 !a
	case 0x4E: {
		std::cout << "NO" << std::endl;
		break;
	}

	// PCALL u
	case 0x4F: {
		uint16_t newPC = ReadAPUMem(PC++);
		newPC |= 0xFF00;

		Push(PC >> 8);
		Push(PC & 0xFF);

		PC = newPC;

		return 6;
	}

	// BVC r
	case 0x50: {
		int8_t offset = ReadAPUMem(PC++);

		if ((PSW & VFlag) != VFlag) {
			PC += offset;

			return 4;

		}

		return 2;
	}

	// TCALL 5
	case 0x51: {
		uint16_t newPC = ReadAPUMem(0xFFD4);
		newPC |= ReadAPUMem(0xFFD5) << 8;

		Push(PC >> 8);
		Push(PC & 0xFF);

		PC = newPC;

		return 8;
	}

	// CLR1 d.2
	case 0x52: {
		std::cout << "NO" << std::endl;
		break;
	}

	// BBC d.2, r
	case 0x53: {
		std::cout << "NO" << std::endl;
		break;
	}

	// EOR A, d+X
	case 0x54: {
		std::cout << "NO" << std::endl;
		break;
	}

	// EOR A, !a+X
	case 0x55: {
		std::cout << "NO" << std::endl;
		break;
	}

	// EOR A, !a+Y
	case 0x56: {
		std::cout << "NO" << std::endl;
		break;
	}

	// EOR A, [d]+Y
	case 0x57: {
		std::cout << "NO" << std::endl;
		break;
	}

	// EOR d, #i
	case 0x58: {
		std::cout << "NO" << std::endl;
		break;
	}

	// EOR (X), (Y)
	case 0x59: {
		std::cout << "NO" << std::endl;
		break;
	}

	// CMPW YA, d
	case 0x5A: {
		std::cout << "NO" << std::endl;
		break;
	}

	// LSR d+X
	case 0x5B: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		uint8_t value = ReadAPUMem(add + X);

		PSW &= ~CFlag;
		if ((value & 0x01) == 0x01) {
			PSW |= CFlag;
		}

		value = value >> 1;

		WriteAPUMem(add + X, value);

		CheckNFlag(value);
		CheckZFlag(value);

		return 5;
	}

	// LSR A
	case 0x5C: {
		PSW &= ~CFlag;
		if ((A & 0x01) == 0x01) {
			PSW |= CFlag;
		}

		A = A >> 1;

		CheckNFlag(A);
		CheckZFlag(A);

		return 2;
	}

	// MOV X, A
	case 0x5D: {
		X = A;

		CheckNFlag(X);
		CheckZFlag(X);

		return 2;
	}

	// CMP Y, !a
	case 0x5E: {
		uint16_t add = ReadAPUMem(PC++);
		add |= ReadAPUMem(PC++);

		uint8_t valueB = ReadAPUMem(add);

		CMP(Y, valueB);

		return 4;
	}

	// JMP !a
	case 0x5F: {
		uint16_t newPC = ReadAPUMem(PC++);
		newPC |= ReadAPUMem(PC++) << 8;

		PC = newPC;

		return 3;
	}

	// CLRC
	case 0x60: {
		PSW &= ~CFlag;
		return 2;
	}

	// TCALL 6
	case 0x61: {
		uint16_t newPC = ReadAPUMem(0xFFD2);
		newPC |= ReadAPUMem(0xFFD3) << 8;

		Push(PC >> 8);
		Push(PC & 0xFF);

		PC = newPC;

		return 8;
	}

	// SET1 d.3
	case 0x62: {
		std::cout << "NO" << std::endl;
		break;
	}

	// BBS d.3, r
	case 0x63: {
		std::cout << "NO" << std::endl;
		break;
	}

	// CMP A, d
	case 0x64: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		uint8_t value = ReadAPUMem(add);

		CMP(A, value);

		return 3;
	}

	// CMP A, !a
	case 0x65: {
		uint16_t add = ReadAPUMem(PC++);
		add |= ReadAPUMem(PC++) << 8;

		uint8_t value = ReadAPUMem(add);

		CMP(A, value);

		return 4;
	}

	// CMP A, (X)
	case 0x66: {
		uint16_t add = X;
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		uint8_t value = ReadAPUMem(add);

		CMP(A, value);

		return 3;
	}

	// CMP A, [d+X]
	case 0x67: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}
		uint16_t trueAdd = ReadAPUMem(add + X);
		trueAdd |= ReadAPUMem(add + X + 1) << 8;

		uint8_t value = ReadAPUMem(trueAdd);

		CMP(A, value);

		return 6;
	}

	// CMP A, #i
	case 0x68: {
		uint8_t value = ReadAPUMem(PC++);

		CMP(A, value);

		return 2;
	}

	// CMP dd, ds
	case 0x69: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		uint8_t valueB = ReadAPUMem(add);

		add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}
		uint8_t valueA = ReadAPUMem(add);

		CMP(valueA, valueB);

		return 6;
	}

	// AND1 C, /m.b
	case 0x6A: {
		std::cout << "NO" << std::endl;
		break;
	}

	// ROR d
	case 0x6B: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		uint8_t value = ReadAPUMem(add);

		uint8_t C = (PSW & CFlag) ? 1 : 0;

		PSW &= ~CFlag;
		if ((A & 0x01) == 0x01) {
			PSW |= CFlag;
		}

		value = value >> 1;

		if (C == 1) {
			value |= 0x80;
		}

		WriteAPUMem(add, value);

		CheckNFlag(value);
		CheckZFlag(value);

		return 4;
	}

	// ROR !a
	case 0x6C: {
		uint16_t add = ReadAPUMem(PC++);
		add |= ReadAPUMem(PC++) << 8;

		uint8_t value = ReadAPUMem(add);

		uint8_t C = (PSW & CFlag) ? 1 : 0;

		PSW &= ~CFlag;
		if ((A & 0x01) == 0x01) {
			PSW |= CFlag;
		}

		value = value >> 1;

		if (C == 1) {
			value |= 0x80;
		}

		WriteAPUMem(add, value);

		CheckNFlag(value);
		CheckZFlag(value);

		return 5;
	}

	// PUSH Y
	case 0x6D: {
		Push(Y);
		return 4;
	}

	// DBNZ d, r
	case 0x6E: {
		std::cout << "NO" << std::endl;
		break;
	}

	// RET
	case 0x6F: {
		PC = Pull();
		PC |= Pull() << 8;

		return 5;
	}

	// BVS r
	case 0x70: {
		int8_t offset = ReadAPUMem(PC++);

		if ((PSW & VFlag) == VFlag) {
			PC += offset;

			return 4;

		}

		return 2;
	}

	// TCALL 7
	case 0x71: {
		uint16_t newPC = ReadAPUMem(0xFFD0);
		newPC |= ReadAPUMem(0xFFD1) << 8;

		Push(PC >> 8);
		Push(PC & 0xFF);

		PC = newPC;

		return 8;
	}

	// CLR1 d.3
	case 0x72: {
		std::cout << "NO" << std::endl;
		break;
	}

	// BBC d.3, r
	case 0x73: {
		std::cout << "NO" << std::endl;
		break;
	}

	// CMP A, d+X
	case 0x74: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		uint8_t value = ReadAPUMem(add + X);

		CMP(A, value);

		return 4;
	}

	// CMP A, !a+X
	case 0x75: {
		uint16_t add = ReadAPUMem(PC++);
		add |= ReadAPUMem(PC++) << 8;

		uint8_t value = ReadAPUMem(add + X);

		CMP(A, value);

		return 5;
	}

	// CMP A, !a+Y
	case 0x76: {
		uint16_t add = ReadAPUMem(PC++);
		add |= ReadAPUMem(PC++) << 8;

		uint8_t value = ReadAPUMem(add + Y);

		CMP(A, value);

		return 5;
	}

	// CMP A, [d]+Y
	case 0x77: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}
		uint16_t trueAdd = ReadAPUMem(add);
		trueAdd |= ReadAPUMem(add + 1) << 8;

		uint8_t value = ReadAPUMem(trueAdd + Y);

		CMP(A, value);

		return 6;
	}

	// CMP d, #i
	case 0x78: {
		uint8_t valueB = ReadAPUMem(PC++);

		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}
		uint8_t valueA = ReadAPUMem(add);

		CMP(valueA, valueB);

		return 5;
	}

	// CMP (X), (Y)
	case 0x79: {
		uint16_t add = X;
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		uint8_t valueA = ReadAPUMem(add);

		add = Y;
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}
		uint8_t valueB = ReadAPUMem(add);

		CMP(valueA, valueB);

		return 5;
	}

	// ADDW YA, d
	case 0x7A: {
		std::cout << "NO" << std::endl;
		break;
	}

	// ROR d+X
	case 0x7B: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		uint8_t value = ReadAPUMem(add + X);

		uint8_t C = (PSW & CFlag) ? 1 : 0;

		PSW &= ~CFlag;
		if ((A & 0x01) == 0x01) {
			PSW |= CFlag;
		}

		value = value >> 1;

		if (C == 1) {
			value |= 0x80;
		}

		WriteAPUMem(add + X, value);

		CheckNFlag(value);
		CheckZFlag(value);

		return 5;
	}

	// ROR A
	case 0x7C: {
		uint8_t C = (PSW & CFlag) ? 1 : 0;

		PSW &= ~CFlag;
		if ((A & 0x01) == 0x01) {
			PSW |= CFlag;
		}

		A = A >> 1;

		if (C == 1) {
			A |= 0x80;
		}

		CheckNFlag(A);
		CheckZFlag(A);

		return 2;
	}

	// MOV A, X
	case 0x7D: {
		A = X;

		CheckNFlag(A);
		CheckZFlag(A);

		return 2;
	}

	// CMP Y, d
	case 0x7E: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		uint8_t valueB = ReadAPUMem(add);

		CMP(Y, valueB);

		return 3;
	}

	// RETI
	case 0x7F: {
		// There are no interrupts

		PSW = Pull();
		PC = Pull();
		PC |= Pull() << 8;

		return 6;
	}

	// SETC
	case 0x80: {
		PSW |= CFlag;
		return 2;
	}

	// TCALL 8
	case 0x81: {
		uint16_t newPC = ReadAPUMem(0xFFCE);
		newPC |= ReadAPUMem(0xFFCF) << 8;

		Push(PC >> 8);
		Push(PC & 0xFF);

		PC = newPC;

		return 8;
	}

	// SET1 d.4
	case 0x82: {
		std::cout << "NO" << std::endl;
		break;
	}

	// BBS d.4, r
	case 0x83: {
		std::cout << "NO" << std::endl;
		break;
	}

	// ADC A, d
	case 0x84: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}
		uint8_t value = ReadAPUMem(add);
		ADC(value);

		return 3;
	}

	// ADC A, !a
	case 0x85: {
		uint16_t add = ReadAPUMem(PC++);
		add |= ReadAPUMem(PC++) << 8;
		uint8_t value = ReadAPUMem(add);
		ADC(value);

		return 4;
	}

	// ADC A, (X)
	case 0x86: {
		uint16_t add = X;
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}
		uint8_t value = ReadAPUMem(add);
		ADC(value);

		return 3;
	}

	// ADC A, [d+X]
	case 0x87: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}
		uint16_t trueAdd = ReadAPUMem(add + X);
		trueAdd |= ReadAPUMem(add + X + 1) << 8;

		uint8_t value = ReadAPUMem(trueAdd);

		ADC(value);

		return 6;
	}

	// ADC A, #i
	case 0x88: {
		uint8_t imm = ReadAPUMem(PC++);
		ADC(imm);

		return 2;
	}

	// ADC dd, ds
	case 0x89: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}
		uint8_t value = ReadAPUMem(add);

		uint8_t prev;
		int8_t C = (PSW & CFlag) ? 1 : 0;

		uint8_t resultAdd = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			resultAdd += 0x100;
		}
		uint8_t result = ReadAPUMem(resultAdd);

		prev = (int8_t)result;

		result = (int8_t)result + (int8_t)value + C;

		if (~(prev ^ value) & (prev ^ result) & 0x80) {
			PSW |= VFlag;
		}
		else {
			PSW &= ~VFlag;
		}

		PSW &= ~HFlag;
		if ((((prev & 0xF) + (value & 0xF) + C) & 0x10) == 0x10) {
			PSW |= HFlag;
		}

		CheckNFlag(result);

		CheckZFlag(result);
		CheckCFlag(result, prev, false);

		WriteAPUMem(resultAdd, result);

		return 6;
	}

	// EOR1 C, m.b
	case 0x8A: {
		std::cout << "NO" << std::endl;
		break;
	}

	// DEC d
	case 0x8B: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}
		uint8_t value = ReadAPUMem(add);
		value--;
		WriteAPUMem(add, value);
		CheckNFlag(value);
		CheckZFlag(value);

		return 4;
	}

	// DEC !a
	case 0x8C: {
		std::cout << "NO" << std::endl;
		break;
	}

	// MOV Y, #i
	case 0x8D: {
		Y = ReadAPUMem(PC++);
		CheckNFlag(Y);
		CheckZFlag(Y);

		return 2;
	}

	// POP PSW
	case 0x8E: {
		PSW = Pull();
		return 4;
	}

	// MOV d, #i
	case 0x8F: {
		uint8_t value = ReadAPUMem(PC++);
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}
		WriteAPUMem(add, value);

		return 5;
	}

	// BCC r
	case 0x90: {
		int8_t offset = ReadAPUMem(PC++);

		if ((PSW & CFlag) != CFlag) {
			PC += offset;

			return 4;

		}

		return 2;
	}

	// TCALL 9
	case 0x91: {
		uint16_t newPC = ReadAPUMem(0xFFCC);
		newPC |= ReadAPUMem(0xFFCD) << 8;

		Push(PC >> 8);
		Push(PC & 0xFF);

		PC = newPC;

		return 8;
	}

	// CLR1 d.4
	case 0x92: {
		std::cout << "NO" << std::endl;
		break;
	}

	// BBC d.4, r
	case 0x93: {
		std::cout << "NO" << std::endl;
		break;
	}

	// ADC A, d+X
	case 0x94: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}
		uint8_t value = ReadAPUMem(add + X);
		ADC(value);

		return 4;
	}

	// ADC A, !a+X
	case 0x95: {
		uint16_t add = ReadAPUMem(PC++);
		add |= ReadAPUMem(PC++) << 8;
		uint8_t value = ReadAPUMem(add + X);
		ADC(value);

		return 5;
	}

	// ADC A, !a+Y
	case 0x96: {
		uint16_t add = ReadAPUMem(PC++);
		add |= ReadAPUMem(PC++) << 8;
		uint8_t value = ReadAPUMem(add + Y);
		ADC(value);

		return 5;
	}

	// ADC A, [d]+Y
	case 0x97: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}
		uint16_t trueAdd = ReadAPUMem(add);
		trueAdd |= ReadAPUMem(add + 1) << 8;

		uint8_t value = ReadAPUMem(trueAdd + Y);

		ADC(value);

		return 6;
	}

	// ADC d, #i
	case 0x98: {
		uint8_t value = ReadAPUMem(PC++);

		uint8_t prev;
		int8_t C = (PSW & CFlag) ? 1 : 0;

		uint8_t resultAdd = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			resultAdd += 0x100;
		}
		uint8_t result = ReadAPUMem(resultAdd);

		prev = (int8_t)result;

		result = (int8_t)result + (int8_t)value + C;

		if (~(prev ^ value) & (prev ^ result) & 0x80) {
			PSW |= VFlag;
		}
		else {
			PSW &= ~VFlag;
		}

		PSW &= ~HFlag;
		if ((((prev & 0xF) + (value & 0xF) + C) & 0x10) == 0x10) {
			PSW |= HFlag;
		}

		CheckNFlag(result);

		CheckZFlag(result);
		CheckCFlag(result, prev, false);

		WriteAPUMem(resultAdd, result);

		return 5;
	}

	// ADC (X), (Y)
	case 0x99: {
		uint16_t add = Y;
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}
		uint8_t value = ReadAPUMem(add);

		uint8_t prev;
		int8_t C = (PSW & CFlag) ? 1 : 0;

		uint8_t resultAdd = X;
		if ((PSW & PFlag) == PFlag) {
			resultAdd += 0x100;
		}
		uint8_t result = ReadAPUMem(resultAdd);

		prev = (int8_t)result;

		result = (int8_t)result + (int8_t)value + C;

		if (~(prev ^ value) & (prev ^ result) & 0x80) {
			PSW |= VFlag;
		}
		else {
			PSW &= ~VFlag;
		}

		PSW &= ~HFlag;
		if ((((prev & 0xF) + (value & 0xF) + C) & 0x10) == 0x10) {
			PSW |= HFlag;
		}

		CheckNFlag(result);

		CheckZFlag(result);
		CheckCFlag(result, prev, false);

		WriteAPUMem(resultAdd, result);

		return 5;
	}

	// SUBW YA, d
	case 0x9A: {
		std::cout << "NO" << std::endl;
		break;
	}

	// DEC d+X
	case 0x9B: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}
		uint8_t value = ReadAPUMem(add + X);
		value--;
		WriteAPUMem(add + X, value);
		CheckNFlag(value);
		CheckZFlag(value);

		return 5;
	}

	// DEC A
	case 0x9C: {
		A--;
		CheckNFlag(A);
		CheckZFlag(A);

		return 2;
	}

	// MOV X, SP
	case 0x9D: {
		X = SP;

		CheckNFlag(X);
		CheckZFlag(X);

		return 2;
	}

	// DIV YA, X
	case 0x9E: {
		uint16_t prod = Y * A;

		// TODO: DIV

		A = prod / X;
		Y = prod % X;

		CheckNFlag(A);
		CheckNFlag(A);

		return 12;
	}

	// XCN A
	case 0x9F: {
		A = (A << 4) | (A >> 4);

		CheckNFlag(A);
		CheckZFlag(A);

		return 5;
	}

	// EI
	case 0xA0: {
		PSW |= IFlag;
		return 3;
	}

	// TCALL 10
	case 0xA1: {
		uint16_t newPC = ReadAPUMem(0xFFCA);
		newPC |= ReadAPUMem(0xFFCB) << 8;

		Push(PC >> 8);
		Push(PC & 0xFF);

		PC = newPC;

		return 8;
	}

	// SET1 d.5
	case 0xA2: {
		std::cout << "NO" << std::endl;
		break;
	}

	// BBS d.5, r
	case 0xA3: {
		std::cout << "NO" << std::endl;
		break;
	}

	// SBC A, d
	case 0xA4: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		uint8_t value = ReadAPUMem(add);

		SBC(value);

		return 3;
	}

	// SBC A, !a
	case 0xA5: {
		uint16_t add = ReadAPUMem(PC++);
		add |= ReadAPUMem(PC++) << 8;

		uint8_t value = ReadAPUMem(add);

		SBC(value);

		return 4;
	}

	// SBC A, (X)
	case 0xA6: {
		uint16_t add = X;
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		uint8_t value = ReadAPUMem(add);

		SBC(value);

		return 3;
	}

	// SBC A, [d+X]	
	case 0xA7: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		uint16_t trueAdd = ReadAPUMem(add + X);
		trueAdd |= ReadAPUMem(add + X + 1) << 8;

		uint8_t value = ReadAPUMem(trueAdd);

		SBC(value);

		return 6;
	}

	// SBC A, #i
	case 0xA8: {
		uint8_t value = ReadAPUMem(PC++);

		SBC(value);

		return 2;
	}

	// SBC dd, ds
	case 0xA9: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}
		uint8_t value = ~ReadAPUMem(add);

		uint8_t prev;
		int8_t C = (PSW & CFlag) ? 1 : 0;

		uint8_t resultAdd = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			resultAdd += 0x100;
		}
		uint8_t resultValue = ReadAPUMem(resultAdd);

		prev = resultValue;

		uint16_t result = resultValue + value + C;

		if (~(prev ^ value) & (prev ^ result) & 0x80) {
			PSW |= VFlag;
		}
		else {
			PSW &= ~VFlag;
		}

		PSW &= ~HFlag;
		if ((((prev & 0xF) + (value & 0xF) + C) & 0x10) == 0x10) {
			PSW |= HFlag;
		}

		CheckNFlag(result);

		CheckZFlag(result);

		if (result > 0xFF) {
			PSW |= CFlag;
		}
		else {
			PSW &= ~CFlag;
		}

		WriteAPUMem(resultAdd, result);

		return 6;
	}

	// MOV1 C, m.b
	case 0xAA: {
		std::cout << "NO" << std::endl;
		break;
	}

	// INC d
	case 0xAB: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}
		uint8_t value = ReadAPUMem(add);
		value++;
		WriteAPUMem(add, value);
		CheckNFlag(value);
		CheckZFlag(value);

		return 4;
	}

	// INC !a
	case 0xAC: {
		uint16_t add = ReadAPUMem(PC++);
		add |= ReadAPUMem(PC++) << 8;
		uint8_t value = ReadAPUMem(add);
		value++;
		WriteAPUMem(add, value);
		CheckNFlag(value);
		CheckZFlag(value);

		return 5;
	}

	// CMP Y, #i
	case 0xAD: {
		uint8_t valueB = ReadAPUMem(PC++);

		CMP(Y, valueB);

		return 2;
	}

	// POP A
	case 0xAE: {
		A = Pull();
		return 4;
	}

	// MOV (X)+, A
	case 0xAF: {
		uint16_t add = X++;
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}
		WriteAPUMem(add, A);

		return 4;
	}

	// BCS r
	case 0xB0: {
		int8_t offset = ReadAPUMem(PC++);

		if ((PSW & CFlag) == CFlag) {
			PC += offset;

			return 4;

		}

		return 2;
	}

	// TCALL 11
	case 0xB1: {
		uint16_t newPC = ReadAPUMem(0xFFC8);
		newPC |= ReadAPUMem(0xFFC9) << 8;

		Push(PC >> 8);
		Push(PC & 0xFF);

		PC = newPC;

		return 8;
	}

	// CLR1 d.5
	case 0xB2: {
		std::cout << "NO" << std::endl;
		break;
	}

	// BBC d.5, r
	case 0xB3: {
		std::cout << "NO" << std::endl;
		break;
	}

	// SBC A, d+X
	case 0xB4: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		uint8_t value = ReadAPUMem(add + X);

		SBC(value);

		return 4;
	}

	// SBC A, !a+X
	case 0xB5: {
		uint16_t add = ReadAPUMem(PC++);
		add |= ReadAPUMem(PC++) << 8;

		uint8_t value = ReadAPUMem(add + X);

		SBC(value);

		return 5;
	}

	// SBC A, !a+Y
	case 0xB6: {
		uint16_t add = ReadAPUMem(PC++);
		add |= ReadAPUMem(PC++) << 8;

		uint8_t value = ReadAPUMem(add + Y);

		SBC(value);

		return 5;
	}

	// SBC A, [d]+Y
	case 0xB7: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		uint16_t trueAdd = ReadAPUMem(add);
		trueAdd |= ReadAPUMem(add + 1) << 8;

		uint8_t value = ReadAPUMem(trueAdd + Y);

		SBC(value);

		return 6;
	}

	// SBC d, #i
	case 0xB8: {
		uint8_t value = ~ReadAPUMem(PC++);

		uint8_t prev;
		int8_t C = (PSW & CFlag) ? 1 : 0;

		uint8_t resultAdd = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			resultAdd += 0x100;
		}
		uint8_t resultValue = ReadAPUMem(resultAdd);

		prev = resultValue;

		uint16_t result = resultValue + value + C;

		if (~(prev ^ value) & (prev ^ result) & 0x80) {
			PSW |= VFlag;
		}
		else {
			PSW &= ~VFlag;
		}

		PSW &= ~HFlag;
		if ((((prev & 0xF) + (value & 0xF) + C) & 0x10) == 0x10) {
			PSW |= HFlag;
		}

		CheckNFlag(result);

		CheckZFlag(result);

		if (result > 0xFF) {
			PSW |= CFlag;
		}
		else {
			PSW &= ~CFlag;
		}

		WriteAPUMem(resultAdd, result);

		return 5;
	}

	// SBC (X), (Y)
	case 0xB9: {
		uint16_t add = Y;
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}
		uint8_t value = ~ReadAPUMem(add);

		uint8_t prev;
		int8_t C = (PSW & CFlag) ? 1 : 0;

		uint8_t resultAdd = X;
		if ((PSW & PFlag) == PFlag) {
			resultAdd += 0x100;
		}
		uint8_t resultValue = ReadAPUMem(resultAdd);

		prev = resultValue;

		uint16_t result = resultValue + value + C;

		if (~(prev ^ value) & (prev ^ result) & 0x80) {
			PSW |= VFlag;
		}
		else {
			PSW &= ~VFlag;
		}

		PSW &= ~HFlag;
		if ((((prev & 0xF) + (value & 0xF) + C) & 0x10) == 0x10) {
			PSW |= HFlag;
		}

		CheckNFlag(result);

		CheckZFlag(result);

		if (result > 0xFF) {
			PSW |= CFlag;
		}
		else {
			PSW &= ~CFlag;
		}

		WriteAPUMem(resultAdd, result);

		return 5;
	}

	// MOVW YA, d
	case 0xBA: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		A = ReadAPUMem(add);
		Y = ReadAPUMem(add + 1);

		CheckNFlag(Y);

		PSW &= ~ZFlag;
		if (A == 0 && Y == 0) {
			PSW |= ZFlag;
		}

		return 5;
	}

	// INC d+X
	case 0xBB: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}
		uint8_t value = ReadAPUMem(add + X);
		value++;
		WriteAPUMem(add + X, value);
		CheckNFlag(value);
		CheckZFlag(value);

		return 5;
	}

	// INC A
	case 0xBC: {
		A++;
		CheckNFlag(A);
		CheckZFlag(A);

		return 2;
	}

	// MOV SP, X
	case 0xBD: {
		SP = X;

		return 2;
	}

	// DAS A
	case 0xBE: {
		std::cout << "NO" << std::endl;
		break;
	}

	// MOV A, (X)+
	case 0xBF: {
		uint16_t add = X++;
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		A = ReadAPUMem(add);
		CheckNFlag(A);
		CheckZFlag(A);

		return 4;
	}

	// DI
	case 0xC0: {
		PSW &= ~IFlag;
		return 3;
	}

	// TCALL 12
	case 0xC1: {
		uint16_t newPC = ReadAPUMem(0xFFC6);
		newPC |= ReadAPUMem(0xFFC7) << 8;

		Push(PC >> 8);
		Push(PC & 0xFF);

		PC = newPC;

		return 8;
	}

	// SET1 d.6
	case 0xC2: {
		std::cout << "NO" << std::endl;
		break;
	}

	// BBS d.6, r
	case 0xC3: {
		std::cout << "NO" << std::endl;
		break;
	}

	// MOV d, A
	case 0xC4: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}
		WriteAPUMem(add, A);

		return 4;
	}

	// MOV !a, A
	case 0xC5: {
		uint16_t add = ReadAPUMem(PC++);
		add |= ReadAPUMem(PC++) << 8;
		WriteAPUMem(add, A);

		return 5;
	}

	// MOV (X), A
	case 0xC6: {
		uint16_t add = X;
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}
		WriteAPUMem(add, A);

		return 4;
	}

	// MOV [d+X], A
	case 0xC7: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}
		uint16_t trueAdd = ReadAPUMem(add + X);
		trueAdd |= ReadAPUMem(add + X + 1) << 8;

		WriteAPUMem(trueAdd, A);

		return 7;
	}

	// CMP X, #i
	case 0xC8: {
		uint8_t valueB = ReadAPUMem(PC++);

		CMP(X, valueB);

		return 2;
	}

	// MOV !a, X
	case 0xC9: {
		uint16_t add = ReadAPUMem(PC++);
		add |= ReadAPUMem(PC++) << 8;
		WriteAPUMem(add, X);

		return 5;
	}

	// MOV1 m.b, C
	case 0xCA: {
		std::cout << "NO" << std::endl;
		break;
	}

	// MOV d, Y
	case 0xCB: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}
		WriteAPUMem(add, Y);

		return 4;
	}

	// MOV !a, Y
	case 0xCC: {
		uint16_t add = ReadAPUMem(PC++);
		add |= ReadAPUMem(PC++) << 8;
		WriteAPUMem(add, Y);

		return 5;
	}

	// MOV X, #i
	case 0xCD: {
		X = ReadAPUMem(PC++);
		CheckNFlag(X);
		CheckZFlag(X);

		return 2;
	}

	// POP X
	case 0xCE: {	
		X = Pull();
		return 4;
	}

	// MUL YA
	case 0xCF: {
		uint16_t result = Y * A;

		A = result & 0xFF;
		Y = (result & 0xFF00) >> 8;

		CheckNFlag(Y);

		PSW &= ~ZFlag;
		if (A == 0 && Y == 0) {
			PSW |= ZFlag;
		}

		return 9;
	}

	// BNE r
	case 0xD0: {
		int8_t offset = ReadAPUMem(PC++);

		if ((PSW & ZFlag) != ZFlag) {
			PC += offset;

			return 4;

		}

		return 2;
	}

	// TCALL 13
	case 0xD1: {
		uint16_t newPC = ReadAPUMem(0xFFC4);
		newPC |= ReadAPUMem(0xFFC5) << 8;

		Push(PC >> 8);
		Push(PC & 0xFF);

		PC = newPC;

		return 8;
	}

	// CLR1 d.6
	case 0xD2: {
		std::cout << "NO" << std::endl;
		break;
	}

	// BBC d.6, r
	case 0xD3: {
		std::cout << "NO" << std::endl;
		break;
	}

	// MOV d+X, A
	case 0xD4: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}
		WriteAPUMem(add + X, A);

		return 5;
	}

	// MOV !a+X, A
	case 0xD5: {
		uint16_t add = ReadAPUMem(PC++);
		add |= ReadAPUMem(PC++) << 8;
		WriteAPUMem(add + X, A);

		return 6;
	}

	// MOV !a+Y, A
	case 0xD6: {
		uint16_t add = ReadAPUMem(PC++);
		add |= ReadAPUMem(PC++) << 8;
		WriteAPUMem(add + Y, A);

		return 6;
	}

	// MOV [d]+Y, A
	case 0xD7: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}
		uint16_t trueAdd = ReadAPUMem(add + X);
		trueAdd |= ReadAPUMem(add + 1) << 8;

		WriteAPUMem(trueAdd + Y, A);

		return 7;
	}

	// MOV d, X
	case 0xD8: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}
		WriteAPUMem(add, X);

		return 4;
	}

	// MOV d+Y, X
	case 0xD9: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}
		WriteAPUMem(add + Y, X);

		return 5;
	}

	// MOVW d, YA
	case 0xDA: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		WriteAPUMem(add, A);
		WriteAPUMem(add + 1, Y);

		return 5;
	}

	// MOV d+X, Y
	case 0xDB: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}
		WriteAPUMem(add + X, Y);

		return 5;
	}

	// DEC Y
	case 0xDC: {
		Y--;
		CheckNFlag(Y);
		CheckZFlag(Y);

		return 2;
	}

	// MOV A, Y
	case 0xDD: {
		A = Y;

		CheckNFlag(A);
		CheckZFlag(A);

		return 2;
	}

	// CBNE d+X, r
	case 0xDE: {
		std::cout << "NO" << std::endl;
		break;
	}

	// DAA A
	case 0xDF: {
		std::cout << "NO" << std::endl;
		break;
	}

	// CLRV
	case 0xE0: {
		PSW &= ~(VFlag | HFlag);
		return 2;
	}

	// TCALL 14
	case 0xE1: {
		uint16_t newPC = ReadAPUMem(0xFFC2);
		newPC |= ReadAPUMem(0xFFC3) << 8;

		Push(PC >> 8);
		Push(PC & 0xFF);

		PC = newPC;

		return 8;
	}

	// SET1 d.7
	case 0xE2: {
		std::cout << "NO" << std::endl;
		break;
	}

	// BBS d.7, r
	case 0xE3: {
		std::cout << "NO" << std::endl;
		break;
	}

	// MOV A, d
	case 0xE4: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		A = ReadAPUMem(add);
		CheckNFlag(A);
		CheckZFlag(A);

		return 3;
	}

	// MOV A, !a
	case 0xE5: {
		uint16_t add = ReadAPUMem(PC++);
		add |= ReadAPUMem(PC++) << 8;

		A = ReadAPUMem(add);
		CheckNFlag(A);
		CheckZFlag(A);

		return 4;
	}

	// MOV A, (X)
	case 0xE6: {
		uint16_t add = X;
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		A = ReadAPUMem(add);
		CheckNFlag(A);
		CheckZFlag(A);

		return 3;
	}

	// MOV A, [d+X]
	case 0xE7: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		uint16_t trueAdd = ReadAPUMem(add + X);
		trueAdd |= ReadAPUMem(add + X + 1) << 8;

		A = ReadAPUMem(trueAdd);
		CheckNFlag(A);
		CheckZFlag(A);

		return 6;
	}

	// MOV A, #i
	case 0xE8: {
		A = ReadAPUMem(PC++);
		CheckNFlag(A);
		CheckZFlag(A);

		return 2;
	}

	// MOV X, !a
	case 0xE9: {
		uint16_t add = ReadAPUMem(PC++);
		add |= ReadAPUMem(PC++) << 8;

		X = ReadAPUMem(add);
		CheckNFlag(X);
		CheckZFlag(X);

		return 4;
	}

	// NOT1 m.b
	case 0xEA: {
		std::cout << "NO" << std::endl;
		break;
	}

	// MOV Y, d
	case 0xEB: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		Y = ReadAPUMem(add);
		CheckNFlag(Y);
		CheckZFlag(Y);

		return 3;
	}

	// MOV Y, !a
	case 0xEC: {
		uint16_t add = ReadAPUMem(PC++);
		add |= ReadAPUMem(PC++) << 8;

		Y = ReadAPUMem(add);
		CheckNFlag(Y);
		CheckZFlag(Y);

		return 4;
	}

	// NOTC
	case 0xED: {
		uint8_t temp = PSW & CFlag;
		if (temp == CFlag) {
			PSW &= ~CFlag;
		}
		else {
			PSW |= CFlag;
		}
		return 3;
	}

	// POP Y
	case 0xEE: {
		Y = Pull();
		return 4;
	}

	// SLEEP
	case 0xEF: {
		// TODO: Sleep
		return 3;
	}

	// BEQ r
	case 0xF0: {
		int8_t offset = ReadAPUMem(PC++);

		if ((PSW & ZFlag) == ZFlag) {
			PC += offset;

			return 4;

		}

		return 2;
	}

	// TCALL 15
	case 0xF1: {
		uint16_t newPC = ReadAPUMem(0xFFC0);
		newPC |= ReadAPUMem(0xFFC1) << 8;

		Push(PC >> 8);
		Push(PC & 0xFF);

		PC = newPC;

		return 8;
	}

	// CLR1 d.7
	case 0xF2: {
		std::cout << "NO" << std::endl;
		break;
	}

	// BBC d.7, r
	case 0xF3: {
		std::cout << "NO" << std::endl;
		break;
	}

	// MOV A, d+X
	case 0xF4: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		A = ReadAPUMem(add + X);
		CheckNFlag(A);
		CheckZFlag(A);

		return 4;
	}

	// MOV A, !a+X
	case 0xF5: {
		uint16_t add = ReadAPUMem(PC++);
		add |= ReadAPUMem(PC++) << 8;

		A = ReadAPUMem(add + X);
		CheckNFlag(A);
		CheckZFlag(A);

		return 5;
	}

	// MOV A, !a+Y
	case 0xF6: {
		uint16_t add = ReadAPUMem(PC++);
		add |= ReadAPUMem(PC++) << 8;

		A = ReadAPUMem(add + Y);
		CheckNFlag(A);
		CheckZFlag(A);

		return 5;
	}

	// MOV A, [d]+Y
	case 0xF7: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		uint16_t trueAdd = ReadAPUMem(add);
		trueAdd |= ReadAPUMem(add + 1) << 8;

		A = ReadAPUMem(trueAdd + Y);
		CheckNFlag(A);
		CheckZFlag(A);

		return 6;
	}

	// MOV X, d
	case 0xF8: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		X = ReadAPUMem(add);
		CheckNFlag(X);
		CheckZFlag(X);

		return 3;
	}

	// MOV X, d+Y
	case 0xF9: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		X = ReadAPUMem(add + Y);
		CheckNFlag(X);
		CheckZFlag(X);

		return 4;
	}

	// MOV dd, ds
	case 0xFA: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}
		uint8_t value = ReadAPUMem(add);
		add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}
		WriteAPUMem(add, value);

		return 5;
	}

	// MOV Y, d+X
	case 0xFB: {
		uint16_t add = ReadAPUMem(PC++);
		if ((PSW & PFlag) == PFlag) {
			add += 0x100;
		}

		Y = ReadAPUMem(add + X);
		CheckNFlag(Y);
		CheckZFlag(Y);

		return 4;
	}

	// INC Y
	case 0xFC: {
		Y++;
		CheckNFlag(Y);
		CheckZFlag(Y);

		return 2;
	}

	// MOV Y, A
	case 0xFD: {
		Y = A;

		CheckNFlag(Y);
		CheckZFlag(Y);

		return 2;
	}

	// DBNZ Y, r
	case 0xFE: {
		std::cout << "NO" << std::endl;
		break;
	}

	// STOP
	case 0xFF: {
		// TODO: Stop
		return 3;
	}
	}

	return 2;
}

uint8_t APU::ReadAPUMem(uint16_t add)
{
	uint8_t value = 0;

	// Page 0
	if (add <= 0x00EF) {
		value = page0[add];
	}
	// Registers
	else if (add <= 0x00FF) {
		if (add >= 0xF2 && add <= 0xF3) {
			value = registers0[add - 0xF0];
		}
		else if (add >= 0xF4 && add <= 0xF7) {
			value = ReadCPUPort(add);
		}
		else if ((add >= 0xF8 && add <= 0xF9) || add >= 0xFD) {
			value = registers1[add - 0xF8];
		}
	}
	// Page 1
	else if (add <= 0x01FF) {
		value = page1[add - 0x100];
	}
	// Memory
	else if (add <= 0xFFBF) {
		value = ARAM[add - 0x200];
	}
	// Memory (read / write)
	else if (add <= 0xFFFF) {
		value = ARAM[add - 0x200];
	}

	return value;
}

void APU::WriteAPUMem(uint16_t add, uint8_t value)
{
	// Page 0
	if (add <= 0x00EF) {
		page0[add] = value;
	}
	// Registers
	else if (add <= 0x00FF) {
		if (add >= 0xF0 && add <= 0xF3) {
			registers0[add - 0xF0] = value;
		}
		else if (add >= 0xF4 && add <= 0xF7) {
			WriteAPUPort(add, value);
		}
		else if (add >= 0xF8 && add <= 0xFC) {
			registers1[add - 0xF8] = value;
		}
	}
	// Page 1
	else if (add <= 0x01FF) {
		page1[add - 0x100] = value;
	}
	// Memory
	else if (add <= 0xFFBF) {
		ARAM[add - 0x200] = value;
	}
	// Memory (read / write)
	else if (add <= 0xFFFF) {
		ARAM[add - 0x200] = value;
	}
}

uint8_t APU::Pull()
{
	uint8_t value = ReadAPUMem(++SP);
	return value;
}

void APU::Push(uint8_t value)
{
	WriteAPUMem(SP--, value);
}

// APU Reading ports
uint8_t APU::ReadCPUPort(uint8_t add)
{
	int index = (add & 0xFF) - 0xF4;

	uint8_t value = 0;

	if (index < 4) {
		value = CPUPorts[index];
	}

	return value;
}

// APU Writting ports
void APU::WriteAPUPort(uint8_t add, uint8_t value)
{
	int index = (add & 0xFF) - 0xF4;

	if (index < 4) {
		APUPorts[index] = value;
	}
}

void APU::CheckNFlag(uint8_t value)
{
	if ((value & 0x80) == 0x80) {
		PSW |= NFlag;
	}
	else {
		PSW &= ~NFlag;
	}
}

void APU::CheckZFlag(uint8_t value)
{
	if (value == 0) {
		PSW |= ZFlag;
	}
	else {
		PSW &= ~ZFlag;
	}
}

void APU::CheckCFlag(uint8_t value, uint8_t prevValue, bool isSub)
{
	if (isSub) {
		if (prevValue < value) {
			PSW |= CFlag;
		}
		else {
			PSW &= ~CFlag;
		}
	}
	else {
		if (prevValue > value) {
			PSW |= CFlag;
		}
		else {
			PSW &= ~CFlag;
		}
	}
}

void APU::ADC(uint8_t value)
{
	uint8_t prevA;
	int8_t C = (PSW & CFlag) ? 1 : 0;

	prevA = (int8_t)A;

	A = (int8_t)A + (int8_t)value + C;

	if (~(prevA ^ value) & (prevA ^ A) & 0x80) {
		PSW |= VFlag;
	}
	else {
		PSW &= ~VFlag;
	}

	PSW &= ~HFlag;
	if ((((prevA & 0xF) + (value & 0xF) + C) & 0x10) == 0x10) {
		PSW |= HFlag;
	}

	CheckNFlag(A);

	CheckZFlag(A);
	CheckCFlag(A, prevA, false);
}

void APU::SBC(uint8_t value)
{
	uint8_t prevA;
	int8_t C = (PSW & CFlag) ? 1 : 0;

	value = ~value;

	prevA = A;

	uint16_t result = A + value + C;

	if (~(prevA ^ value) & (prevA ^ A) & 0x80) {
		PSW |= VFlag;
	}
	else {
		PSW &= ~VFlag;
	}

	A = result;

	PSW &= ~HFlag;
	if ((((prevA & 0xF) + (value & 0xF) + C) & 0x10) == 0x10) {
		PSW |= HFlag;
	}

	CheckNFlag(A);

	CheckZFlag(A);
	
	if (result > 0xFF) {
		PSW |= CFlag;
	}
	else {
		PSW &= ~CFlag;
	}
}

void APU::CMP(uint8_t valueA, uint8_t valueB)
{
	if (valueA == valueB) {
		PSW |= ZFlag;
	}
	else {
		PSW &= ~ZFlag;
	}

	if (valueA < valueB) {
		PSW &= ~CFlag;
	}
	else {
		PSW |= CFlag;
	}

	uint8_t value = valueA - valueB;

	CheckNFlag(value);
}

// CPU Writting ports
void APU::WriteCPUPort(uint8_t add, uint8_t value)
{
	int index = add & 0xF;

	if (index < 4) {
		CPUPorts[index] = value;
	}
}

// CPU Reading ports
uint8_t APU::ReadAPUPort(uint8_t add)
{
	int index = add & 0xF;

	uint8_t value = 0;

	if (index < 4) {
		value = APUPorts[index];
	}

	return value;
}
