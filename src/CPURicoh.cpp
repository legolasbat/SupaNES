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

int CPURicoh::Clock()
{
	if (cycles == 0) {
		opcode = ReadMemory(PC++);
		//std::cout << std::hex << (int)opcode << std::endl;

		cycles = Execute();

		Debug();
	}
	
	cycles--;

	return 0;
}

void CPURicoh::Debug() {
	std::cout << std::setfill('0');
	std::cout << std::hex << std::setw(6) << PC << " ";
	std::cout << "A:" << std::setw(4) << A << " ";
	std::cout << "X:" << std::setw(4) << X << " ";
	std::cout << "Y:" << std::setw(4) << Y << " ";
	std::cout << "S:" << std::setw(4) << SP << " ";
	std::cout << "D:" << std::setw(4) << DP << " ";
	std::cout << "DB:" << std::setw(2) << DB << " ";

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

	std::cout << std::endl;

}

uint8_t CPURicoh::ReadMemory(uint32_t add)
{
	return mem->ReadMemory(add);
}

void CPURicoh::WriteMemory(uint32_t add, uint8_t value)
{
	mem->WriteMemory(add, value);
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

		break;

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
	case 0x10:

		break;

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

		CheckNFlag(A);
		CheckZFlag(A);

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
	case 0x20:

		break;

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
	case 0x24:

		break;

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
	case 0x29:

		break;

		// ROL A
	case 0x2A:

		break;

		// PLD
	case 0x2B:

		break;

		// BIT addr
	case 0x2C:

		break;

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

		CheckNFlag(A);
		CheckZFlag(A);

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

		break;

		// EOR #const
	case 0x49:

		break;

		// LSR A
	case 0x4A:

		break;

		// PHK
	case 0x4B:

		break;

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

		break;

		// TCD
	case 0x5B:

		break;

		// JMP long
	case 0x5C:

		break;

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
	case 0x60:

		break;

		// ADC (dp, X)
	case 0x61:

		break;

		// PER label
	case 0x62:

		break;

		// ADC sr, S
	case 0x63:

		break;

		// STZ dp
	case 0x64:

		break;

		// ADC dp
	case 0x65:

		break;

		// ROR dp
	case 0x66:

		break;

		// ADC [dp]
	case 0x67:

		break;

		// PLA
	case 0x68:

		break;

		// ADC #const
	case 0x69:

		break;

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
	case 0x6D:

		break;

		// ROR addr
	case 0x6E:

		break;

		// ADC long
	case 0x6F:

		break;

		// BVS nearlabel
	case 0x70:

		break;

		// ADC (dp), Y
	case 0x71:

		break;

		// ADC (dp)
	case 0x72:

		break;

		// ADC (sr, S), Y
	case 0x73:

		break;

		// STZ dp, X
	case 0x74:

		break;

		// ADC dp, X
	case 0x75:

		break;

		// ROR dp, X
	case 0x76:

		break;

		// ADC [dp], Y
	case 0x77:

		break;

		// SEI
	case 0x78:

		P |= IFlag;

		return 2;

		// ADC addr, Y
	case 0x79:

		break;

		// PLY
	case 0x7A:

		break;

		// TDC
	case 0x7B:

		break;

		// JMP (addr, X)
	case 0x7C:

		break;

		// ADC addr, X
	case 0x7D:

		break;

		// ROR addr, X
	case 0x7E:

		break;

		// ADC long, X
	case 0x7F:

		break;

		// BRA nearlabel
	case 0x80:

		break;

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
	case 0x85:

		break;

		// STX dp
	case 0x86:

		break;

		// STA [dp]
	case 0x87:

		break;

		// DEY
	case 0x88:

		Y--;

		CheckNFlag(Y);
		CheckZFlag(Y);

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
	case 0x8C:

		break;

		// STA addr
	case 0x8D:

		break;

		// STX addr
	case 0x8E:

		break;

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

		break;

		// TXY
	case 0x9B:

		break;

		// STZ addr
	case 0x9C:

		break;

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

		break;

		// LDA (dp, X)
	case 0xA1:

		break;

		// LDX #const
	case 0xA2:

		break;

		// LDA sr, S
	case 0xA3:

		break;

		// LDY dp
	case 0xA4:

		break;

		// LDA dp
	case 0xA5:

		break;

		// LDX dp
	case 0xA6:

		break;

		// LDA [dp]
	case 0xA7:

		break;

		// TAY
	case 0xA8:

		break;

		// LDA #const
	case 0xA9:

		break;

		// TAX
	case 0xAA:

		break;

		// PLB
	case 0xAB:

		break;

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
	case 0xBD:

		break;

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

		uint8_t value = ReadMemory(PC++);
		P &= ~value;

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

		CheckNFlag(Y);
		CheckZFlag(Y);

		return 2;

		// CMP #const
	case 0xC9:

		break;

		// DEX
	case 0xCA:

		X--;

		CheckNFlag(X);
		CheckZFlag(X);

		return 2;

		// WAI
	case 0xCB:

		break;

		// CPY addr
	case 0xCC:

		break;

		// CMP addr
	case 0xCD:

		break;

		// DEC addr
	case 0xCE:

		break;

		// CMP long
	case 0xCF:

		break;

		// BNE nearlabel
	case 0xD0:

		break;

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

		break;

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
	case 0xE0:

		break;

		// SBC (dp, X)
	case 0xE1:

		break;

		// SEP #const
	case 0xE2: {

		uint8_t value = ReadMemory(PC++);
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

		break;

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

		CheckNFlag(A);
		CheckZFlag(A);

		return 3;
	}

		// CPX addr
	case 0xEC:

		break;

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
	case 0xF0:

		break;

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

		break;

		// XCE
	case 0xFB:

		break;

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

void CPURicoh::CheckNFlag(uint16_t value) {

	if ((value & 0x8000) == 0x8000) {
		P |= NFlag;
	}
	else {
		P &= ~NFlag;
	}
}

void CPURicoh::CheckZFlag(uint16_t value) {

	if (value == 0) {
		P |= CFlag;
	}
	else {
		P &= ~CFlag;
	}
}
