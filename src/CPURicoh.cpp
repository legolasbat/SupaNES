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
		if (freopen_s(&LOG, "TestMOV.txt", "a", stdout) == NULL) {
			started = true;
			WriteMemory(0x0, 0xB5, true);
		}
	}

	if (cycles == 0) {
		opcode = ReadMemory((PB << 16) | PC++, true);
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

	if (lines >= 7277) {
		std::cout << " ";
		fclose(stdout);
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

	uint8_t value = ReadMemory(++SP, true);
	//std::cout << "Pulling " << std::hex << (int)value << " at " << std::hex << (int)SP << std::endl;
	return value;
}

void CPURicoh::Push(uint8_t value) {
	//std::cout << "Pushing " << std::hex << (int)value << " at " << std::hex << (int)SP << std::endl;
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
	case 0x00: {

		if(!emulationMode)
			Push(PB);

		PC++;
		Push(PC >> 8);
		Push(PC & 0xFF);

		Push(P);

		P |= IFlag;
		P &= ~DFlag;

		PB = 0;

		uint16_t add = ReadMemory(0xFFE6, true);
		add |= ReadMemory(0xFFE7, true) << 8;

		PC = add;

		return emulationMode ? 7 : 8;
	}
		// ORA (dp, X)
	case 0x01: {

		uint16_t value = GetValue(AddMode::DirectIndexedIndirect, !(P & MFlag));

		ORA(value);

		return (!(P & MFlag) ? 7 : 6) + extraCycles;
	}
		// COP const
	case 0x02: {

		if (!emulationMode)
			Push(PB);

		PC++;
		Push(PC >> 8);
		Push(PC & 0xFF);

		Push(P);

		P |= IFlag;
		P &= ~DFlag;

		PB = 0;

		uint16_t add = ReadMemory(0xFFE4, true);
		add |= ReadMemory(0xFFE5, true) << 8;

		PC = add;

		return emulationMode ? 7 : 8;
	}
		// ORA sr, S
	case 0x03: {

		uint16_t value = GetValue(AddMode::StackRelative, !(P & MFlag));

		ORA(value);

		return !(P & MFlag) ? 5 : 4;
	}
		// TSB dp
	case 0x04: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= DP << 8;

		int c = 0;
		if (DP & 0xFF) {
			c++;
		}

		uint16_t value = ReadMemory(add, true);

		if (!(P & MFlag)) {
			value |= ReadMemory(add + 1, true) << 8;
			c += 2;
		}

		uint16_t result = value | A;

		CheckZFlag(value & A, true, false);

		WriteMemory(add, (result & 0xFF), true);

		if (!(P & MFlag)) {
			WriteMemory(add + 1, (result & 0xFF00) >> 8, true);
		}

		return 5 + c;
	}

		break;

		// ORA dp
	case 0x05: {

		uint16_t value = GetValue(AddMode::Direct, !(P & MFlag));

		ORA(value);

		return (!(P & MFlag) ? 4 : 3) + extraCycles;
	}
		// ASL dp
	case 0x06: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= DP << 8;

		int c = 0;
		if (DP & 0xFF) {
			c++;
		}

		uint16_t value = ReadMemory(add, true);

		if (!(P & MFlag)) {
			value |= ReadMemory(add + 1, true) << 8;
			c += 2;
		}

		uint16_t prev = value;

		bool M = P & MFlag;

		value <<= 1;
		if (M) {

			if (prev & 0x0080) {
				P |= CFlag;
			}
			else {
				P &= ~CFlag;
			}
		}
		else {

			if (prev & 0x8000) {
				P |= CFlag;
			}
			else {
				P &= ~CFlag;
			}
		}

		if ((M && ((value & 0xFF) == 0)) || (!M && (value == 0))) {
			P |= ZFlag;
		}
		else {
			P &= ~ZFlag;
		}

		if ((M && ((value & 0xFF) & 0x80)) || (!M && (value & 0x8000))) {
			P |= NFlag;
		}
		else {
			P &= ~NFlag;
		}

		WriteMemory(add, (value & 0xFF), true);
		if (!M) {
			WriteMemory(add + 1, value >> 8, true);
		}

		return 5 + c;
	}
		// ORA [dp]
	case 0x07: {

		uint16_t value = GetValue(AddMode::DirectIndirectLong, !(P & MFlag));

		ORA(value);

		return (!(P & MFlag) ? 7 : 6) + extraCycles;
	}
		// PHP
	case 0x08:

		Push(P);

		return 3;

		// ORA #const
	case 0x09: {

		uint16_t value = GetValue(AddMode::Immediate, !(P & MFlag));

		ORA(value);

		return !(P & MFlag) ? 3 : 2;
	}
		// ASL A
	case 0x0A: {

		uint16_t prevA = A;

		bool M = P & MFlag;

		if (M) {
			uint8_t l = A & 0xFF;
			A &= 0xFF00;
			A |= (l << 1) & 0xFF;

			if (prevA & 0x0080) {
				P |= CFlag;
			}
			else {
				P &= ~CFlag;
			}
		}
		else {
			A <<= 1;

			if (prevA & 0x8000) {
				P |= CFlag;
			}
			else {
				P &= ~CFlag;
			}
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

		return 2;
	}
		// PHD
	case 0x0B:

		Push(DP >> 8);
		Push(DP & 0xFF);

		return 4;

		// TSB addr
	case 0x0C: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;

		uint16_t value = ReadMemory(add, false);

		int c = 0;
		if (!(P & MFlag)) {
			value |= ReadMemory(add + 1, false) << 8;
			c += 2;
		}

		uint16_t result = value | A;

		CheckZFlag(value & A, true, false);

		WriteMemory(add, (result & 0xFF), false);

		if (!(P & MFlag)) {
			WriteMemory(add + 1, (result & 0xFF00) >> 8, false);
		}

		return 6 + c;
	}
		// ORA addr
	case 0x0D: {

		uint16_t value = GetValue(AddMode::Absolute, !(P & MFlag));

		ORA(value);

		return !(P & MFlag) ? 5 : 4;
	}
		// ASL addr
	case 0x0E: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;

		uint16_t value = ReadMemory(add, false);

		if (!(P & MFlag)) {
			value |= ReadMemory(add + 1, false) << 8;
		}

		uint16_t prev = value;

		bool M = P & MFlag;

		value <<= 1;
		if (M) {

			if (prev & 0x0080) {
				P |= CFlag;
			}
			else {
				P &= ~CFlag;
			}
		}
		else {

			if (prev & 0x8000) {
				P |= CFlag;
			}
			else {
				P &= ~CFlag;
			}
		}

		if ((M && ((value & 0xFF) == 0)) || (!M && (value == 0))) {
			P |= ZFlag;
		}
		else {
			P &= ~ZFlag;
		}

		if ((M && ((value & 0xFF) & 0x80)) || (!M && (value & 0x8000))) {
			P |= NFlag;
		}
		else {
			P &= ~NFlag;
		}

		WriteMemory(add, (value & 0xFF), false);
		if (!M) {
			WriteMemory(add + 1, value >> 8, false);
		}

		return M ? 6 : 8;
	}
		// ORA long
	case 0x0F: {

		uint16_t value = GetValue(AddMode::AbsoluteLong, !(P & MFlag));

		ORA(value);

		return !(P & MFlag) ? 6 : 5;
	}
		// BPL nearlabel
	case 0x10: {

		int8_t offset = ReadMemory((PB << 16) | PC++, true);

		if (!(P & NFlag)) {

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
		// ORA (dp), Y
	case 0x11: {

		uint16_t value = GetValue(AddMode::DirectIndirectIndexed, !(P & MFlag));

		ORA(value);

		return (!(P & MFlag) ? 6 : 5) + extraCycles;
	}
		// ORA (dp)
	case 0x12: {

		uint16_t value = GetValue(AddMode::DirectIndirect, !(P & MFlag));

		ORA(value);

		return (!(P & MFlag) ? 6 : 5) + extraCycles;
	}
		// ORA (sr, S), Y
	case 0x13: {

		uint16_t value = GetValue(AddMode::StackRelativeIndirectIndexed, !(P & MFlag));

		ORA(value);

		return !(P & MFlag) ? 8 : 7;
	}
		// TRB dp
	case 0x14: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= DP << 8;

		int c = 0;
		if (DP & 0xFF) {
			c++;
		}

		uint16_t value = ReadMemory(add, true);

		uint16_t result = 0;

		if (!(P & MFlag)) {
			value |= ReadMemory(add + 1, true) << 8;
			c += 2;
			result = value & ~A;
		}
		else {
			result = value & ~(A & 0x00FF);
		}

		CheckZFlag(value & A, true, false);

		WriteMemory(add, (result & 0xFF), true);

		if (!(P & MFlag)) {
			WriteMemory(add + 1, (result & 0xFF00) >> 8, true);
		}

		return 5 + c;
	}
		// ORA dp, X
	case 0x15: {

		uint16_t value = GetValue(AddMode::DirectIndexedX, !(P & MFlag));

		ORA(value);

		return (!(P & MFlag) ? 5 : 4) + extraCycles;
	}
		// ASL dp, X
	case 0x16: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= DP << 8;
		add += X;

		int c = 0;
		if (DP & 0xFF) {
			c++;
		}

		uint16_t value = ReadMemory(add, true);

		if (!(P & MFlag)) {
			value |= ReadMemory(add + 1, true) << 8;
			c += 2;
		}

		uint16_t prev = value;

		bool M = P & MFlag;

		value <<= 1;
		if (M) {

			if (prev & 0x0080) {
				P |= CFlag;
			}
			else {
				P &= ~CFlag;
			}
		}
		else {

			if (prev & 0x8000) {
				P |= CFlag;
			}
			else {
				P &= ~CFlag;
			}
		}

		if ((M && ((value & 0xFF) == 0)) || (!M && (value == 0))) {
			P |= ZFlag;
		}
		else {
			P &= ~ZFlag;
		}

		if ((M && ((value & 0xFF) & 0x80)) || (!M && (value & 0x8000))) {
			P |= NFlag;
		}
		else {
			P &= ~NFlag;
		}

		WriteMemory(add, (value & 0xFF), true);
		if (!M) {
			WriteMemory(add + 1, value >> 8, true);
		}

		return 5 + c;
	}
		// ORA [dp], Y
	case 0x17: {

		uint16_t value = GetValue(AddMode::DirectIndirectLong, !(P & MFlag));

		ORA(value);

		return (!(P & MFlag) ? 7 : 6) + extraCycles;
	}

		// CLC
	case 0x18:

		P &= ~CFlag;

		return 2;

		// ORA addr, Y
	case 0x19: {

		uint16_t value = GetValue(AddMode::AbsoluteIndexedY, !(P & MFlag));

		ORA(value);

		return (!(P & MFlag) ? 5 : 4) + extraCycles;
	}
		// INC A
	case 0x1A:

		if (!(P & MFlag)) {
			A++;
		}
		else {
			uint8_t l = A & 0xFF;
			l++;
			A &= 0xFF00;
			A |= l;
		}

		CheckNFlag(A, true, false);
		CheckZFlag(A, true, false);

		return 2;

		// TCS
	case 0x1B:

		SP = A;

		return 2;

		// TRB addr
	case 0x1C: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;

		uint16_t value = ReadMemory(add, false);

		uint16_t result = 0;

		int c = 0;
		if (!(P & MFlag)) {
			value |= ReadMemory(add + 1, false) << 8;
			c += 2;
			result = value & ~A;
		}
		else {
			result = value & ~(A & 0x00FF);
		}

		CheckZFlag(value & A, true, false);

		WriteMemory(add, (result & 0xFF), false);

		if (!(P & MFlag)) {
			WriteMemory(add + 1, (result & 0xFF00) >> 8, false);
		}

		return 6 + c;
	}
		// ORA addr, X
	case 0x1D: {

		uint16_t value = GetValue(AddMode::AbsoluteIndexedX, !(P & MFlag));

		ORA(value);

		return (!(P & MFlag) ? 5 : 4) + extraCycles;
	}
		// ASL addr, X
	case 0x1E: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;
		add += X;

		uint16_t value = ReadMemory(add, false);

		if (!(P & MFlag)) {
			value |= ReadMemory(add + 1, false) << 8;
		}

		uint16_t prev = value;

		bool M = P & MFlag;

		value <<= 1;
		if (M) {

			if (prev & 0x0080) {
				P |= CFlag;
			}
			else {
				P &= ~CFlag;
			}
		}
		else {

			if (prev & 0x8000) {
				P |= CFlag;
			}
			else {
				P &= ~CFlag;
			}
		}

		if ((M && ((value & 0xFF) == 0)) || (!M && (value == 0))) {
			P |= ZFlag;
		}
		else {
			P &= ~ZFlag;
		}

		if ((M && ((value & 0xFF) & 0x80)) || (!M && (value & 0x8000))) {
			P |= NFlag;
		}
		else {
			P &= ~NFlag;
		}

		WriteMemory(add, (value & 0xFF), false);
		if (!M) {
			WriteMemory(add + 1, value >> 8, false);
		}

		return M ? 7 : 9;
	}
		// ORA long, X
	case 0x1F: {

		uint16_t value = GetValue(AddMode::AbsoluteIndexedLong, !(P & MFlag));

		ORA(value);

		return !(P & MFlag) ? 6 : 5;
	}
		// JSR addr
	case 0x20: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;

		Push((PC & 0xFF00) >> 8);
		Push(PC & 0xFF);

		PC = add;

		return 6;
	}
		// AND (dp, X)
	case 0x21: {

		uint16_t value = GetValue(AddMode::DirectIndexedIndirect, !(P & MFlag));

		AND(value);

		return (!(P & MFlag) ? 7 : 6) + extraCycles;
	}
		// JSR long
	case 0x22: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;
		PB = ReadMemory((PB << 16) | PC++, true);

		Push(PB);
		Push((PC & 0xFF00) >> 8);
		Push(PC & 0xFF);

		PC = add;

		return 8;
	}
		// AND sr, S
	case 0x23: {

		uint16_t value = GetValue(AddMode::StackRelative, !(P & MFlag));

		AND(value);

		return !(P & MFlag) ? 5 : 4;
	}
		// BIT dp
	case 0x24: {

		uint16_t value = GetValue(AddMode::Direct, !(P & MFlag));
		uint16_t result;

		int c = 0;

		if (!(P & MFlag)) {
			c++;

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
	case 0x25: {

		uint16_t value = GetValue(AddMode::Direct, !(P & MFlag));

		AND(value);

		return (!(P & MFlag) ? 4 : 3) + extraCycles;
	}
		// ROL dp
	case 0x26: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= DP << 8;

		int c = 0;
		if (DP & 0xFF) {
			c++;
		}

		uint16_t value = ReadMemory(add, true);

		if (!(P & MFlag)) {
			value |= ReadMemory(add + 1, true) << 8;
			c += 2;
		}

		uint16_t prev = value;

		bool M = P & MFlag;

		value <<= 1;

		if (P & CFlag) {
			value |= 1;
		}

		if (M) {

			if (prev & 0x0080) {
				P |= CFlag;
			}
			else {
				P &= ~CFlag;
			}
		}
		else {

			if (prev & 0x8000) {
				P |= CFlag;
			}
			else {
				P &= ~CFlag;
			}
		}

		if ((M && ((value & 0xFF) == 0)) || (!M && (value == 0))) {
			P |= ZFlag;
		}
		else {
			P &= ~ZFlag;
		}

		if ((M && ((value & 0xFF) & 0x80)) || (!M && (value & 0x8000))) {
			P |= NFlag;
		}
		else {
			P &= ~NFlag;
		}

		WriteMemory(add, (value & 0xFF), true);
		if (!M) {
			WriteMemory(add + 1, value >> 8, true);
		}

		return 5 + c;
	}
		// AND [dp]
	case 0x27: {

		uint16_t value = GetValue(AddMode::DirectIndirectLong, !(P & MFlag));

		AND(value);

		return (!(P & MFlag) ? 7 : 6) + extraCycles;
	}

		break;

		// PLP
	case 0x28:

		P = Pull();

		if (P & XFlag) {
			X &= 0x00FF;
			Y &= 0x00FF;
		}

		return 4;

		// AND #const
	case 0x29: {

		uint16_t value = GetValue(AddMode::Immediate, !(P & MFlag));

		AND(value);

		return !(P & MFlag) ? 3 : 2;
	}
		// ROL A
	case 0x2A: {

		uint16_t prevA = A;

		bool M = P & MFlag;

		if (M) {
			uint8_t l = A & 0xFF;
			A &= 0xFF00;
			A |= (l << 1) & 0xFF;

			if (P & CFlag) {
				A |= 1;
			}

			if (prevA & 0x0080) {
				P |= CFlag;
			}
			else {
				P &= ~CFlag;
			}
		}
		else {
			A <<= 1;

			if (P & CFlag) {
				A |= 1;
			}

			if (prevA & 0x8000) {
				P |= CFlag;
			}
			else {
				P &= ~CFlag;
			}
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

		return 2;
	}
		// PLD
	case 0x2B:

		DP = Pull();
		DP |= Pull() << 8;

		if (DP & 0x8000) {
			P |= NFlag;
		}
		else {
			P &= ~NFlag;
		}

		if (DP == 0) {
			P |= ZFlag;
		}
		else {
			P &= ~ZFlag;
		}

		return 5;

		// BIT addr
	case 0x2C: {

		uint16_t value = GetValue(AddMode::Absolute, !(P & MFlag));
		uint16_t result;

		if (!(P & MFlag)) {

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
	case 0x2D: {

		uint16_t value = GetValue(AddMode::Absolute, !(P & MFlag));

		AND(value);

		return !(P & MFlag) ? 5 : 4;
	}
		// ROL addr
	case 0x2E: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;

		uint16_t value = ReadMemory(add, false);

		if (!(P & MFlag)) {
			value |= ReadMemory(add + 1, false) << 8;
		}

		uint16_t prev = value;

		bool M = P & MFlag;

		value <<= 1;

		if (P & CFlag) {
			value |= 1;
		}

		if (M) {

			if (prev & 0x0080) {
				P |= CFlag;
			}
			else {
				P &= ~CFlag;
			}
		}
		else {

			if (prev & 0x8000) {
				P |= CFlag;
			}
			else {
				P &= ~CFlag;
			}
		}

		if ((M && ((value & 0xFF) == 0)) || (!M && (value == 0))) {
			P |= ZFlag;
		}
		else {
			P &= ~ZFlag;
		}

		if ((M && ((value & 0xFF) & 0x80)) || (!M && (value & 0x8000))) {
			P |= NFlag;
		}
		else {
			P &= ~NFlag;
		}

		WriteMemory(add, (value & 0xFF), false);
		if (!M) {
			WriteMemory(add + 1, value >> 8, false);
		}

		return M ? 6 : 8;
	}
		// AND long
	case 0x2F: {

		uint16_t value = GetValue(AddMode::AbsoluteLong, !(P & MFlag));

		AND(value);

		return !(P & MFlag) ? 6 : 5;
	}
		// BMI nearlabel
	case 0x30: {

		int8_t offset = ReadMemory((PB << 16) | PC++, true);

		if (P & NFlag) {

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
		// AND (dp), Y
	case 0x31: {

		uint16_t value = GetValue(AddMode::DirectIndirectIndexed, !(P & MFlag));

		AND(value);

		return (!(P & MFlag) ? 6 : 5) + extraCycles;
	}
		// AND (dp)
	case 0x32: {

		uint16_t value = GetValue(AddMode::DirectIndirect, !(P & MFlag));

		AND(value);

		return (!(P & MFlag) ? 6 : 5) + extraCycles;
	}
		// AND (sr, S), Y
	case 0x33: {

		uint16_t value = GetValue(AddMode::StackRelativeIndirectIndexed, !(P & MFlag));

		AND(value);

		return !(P & MFlag) ? 8 : 7;
	}
		// BIT dp, X
	case 0x34: {

		uint16_t value = GetValue(AddMode::DirectIndexedX, !(P & MFlag));
		uint16_t result;

		int c = 0;

		if (!(P & MFlag)) {
			c++;

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

		return 4 + c;
	}
		// AND dp, X
	case 0x35: {

		uint16_t value = GetValue(AddMode::DirectIndexedX, !(P & MFlag));

		AND(value);

		return (!(P & MFlag) ? 5 : 4) + extraCycles;
	}
		// ROL dp, X
	case 0x36: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= DP << 8;
		add += X;

		int c = 0;
		if (DP & 0xFF) {
			c++;
		}

		uint16_t value = ReadMemory(add, true);

		if (!(P & MFlag)) {
			value |= ReadMemory(add + 1, true) << 8;
			c += 2;
		}

		uint16_t prev = value;

		bool M = P & MFlag;

		value <<= 1;
		if (P & CFlag) {
			value |= 1;
		}

		if (M) {

			if (prev & 0x0080) {
				P |= CFlag;
			}
			else {
				P &= ~CFlag;
			}
		}
		else {

			if (prev & 0x8000) {
				P |= CFlag;
			}
			else {
				P &= ~CFlag;
			}
		}

		if ((M && ((value & 0xFF) == 0)) || (!M && (value == 0))) {
			P |= ZFlag;
		}
		else {
			P &= ~ZFlag;
		}

		if ((M && ((value & 0xFF) & 0x80)) || (!M && (value & 0x8000))) {
			P |= NFlag;
		}
		else {
			P &= ~NFlag;
		}

		WriteMemory(add, (value & 0xFF), true);
		if (!M) {
			WriteMemory(add + 1, value >> 8, true);
		}

		return 5 + c;
	}
		// AND [dp], Y
	case 0x37: {

		uint16_t value = GetValue(AddMode::DirectIndirectLong, !(P & MFlag));

		AND(value);

		return (!(P & MFlag) ? 7 : 6) + extraCycles;
	}
		// SEC
	case 0x38:

		P |= CFlag;

		return 2;

		// AND addr, Y
	case 0x39: {

		uint16_t value = GetValue(AddMode::AbsoluteIndexedY, !(P & MFlag));

		AND(value);

		return (!(P & MFlag) ? 5 : 4) + extraCycles;
	}
		// DEC A
	case 0x3A:

		if(!(P & MFlag)){
			A--;
		}
		else {
			uint8_t l = A & 0xFF;
			l--;
			A &= 0xFF00;
			A |= l;
		}

		CheckNFlag(A, true, false);
		CheckZFlag(A, true, false);

		return 2;

		// TSC
	case 0x3B:

		A = SP;

		CheckNFlag(A, true, false);
		CheckZFlag(A, true, false);

		return 2;

		// BIT addr, X
	case 0x3C: {

		uint16_t value = GetValue(AddMode::AbsoluteIndexedX, !(P & MFlag));
		uint16_t result;

		if (!(P & MFlag)) {

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

		return (!(P & MFlag) ? 5 : 4) + extraCycles;
	}
		// AND addr, X
	case 0x3D: {

		uint16_t value = GetValue(AddMode::AbsoluteIndexedX, !(P & MFlag));

		AND(value);

		return (!(P & MFlag) ? 5 : 4) + extraCycles;
	}

		break;

		// ROL addr, X
	case 0x3E: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;
		add += X;

		uint16_t value = ReadMemory(add, false);

		if (!(P & MFlag)) {
			value |= ReadMemory(add + 1, false) << 8;
		}

		uint16_t prev = value;

		bool M = P & MFlag;

		value <<= 1;
		if (P & CFlag) {
			value |= 1;
		}

		if (M) {

			if (prev & 0x0080) {
				P |= CFlag;
			}
			else {
				P &= ~CFlag;
			}
		}
		else {

			if (prev & 0x8000) {
				P |= CFlag;
			}
			else {
				P &= ~CFlag;
			}
		}

		if ((M && ((value & 0xFF) == 0)) || (!M && (value == 0))) {
			P |= ZFlag;
		}
		else {
			P &= ~ZFlag;
		}

		if ((M && ((value & 0xFF) & 0x80)) || (!M && (value & 0x8000))) {
			P |= NFlag;
		}
		else {
			P &= ~NFlag;
		}

		WriteMemory(add, (value & 0xFF), false);
		if (!M) {
			WriteMemory(add + 1, value >> 8, false);
		}

		return M ? 7 : 9;
	}
		// AND long, X
	case 0x3F: {

		uint16_t value = GetValue(AddMode::AbsoluteIndexedLong, !(P & MFlag));

		AND(value);

		return !(P & MFlag) ? 6 : 5;
	}
		// RTI
	case 0x40:

		P = Pull();

		PC = Pull();
		PC |= Pull() << 8;

		if (!emulationMode)
			PB = Pull();

		return emulationMode ? 7 : 8;

		// EOR (dp, X)
	case 0x41: {

		uint16_t value = GetValue(AddMode::DirectIndexedIndirect, !(P & MFlag));

		EOR(value);

		return (!(P & MFlag) ? 7 : 6) + extraCycles;
	}
		// WDM
	case 0x42:

		PC++;

		return 2;

		// EOR sr, S
	case 0x43: {

		uint16_t value = GetValue(AddMode::StackRelative, !(P & MFlag));

		EOR(value);

		return (!(P & MFlag) ? 5 : 4);
	}
		// MVP srcbk, destbk
	case 0x44: {

		if (!movActive) {
			srcbk = ReadMemory((PB << 16) | PC++, true);
			destbk = ReadMemory((PB << 16) | PC++, true);
			PC -= 2;
		}

		uint8_t value = ReadMemory((srcbk << 16) | X, true);
		WriteMemory((destbk << 16) | Y, value, true);

		A--;
		X--;
		Y--;

		if (A == 0xFFFF) {
			movActive = false;
			PC += 2;
		}
		else {
			movActive = true;
			PC--;
		}

		return 7;
	}
		// EOR dp
	case 0x45: {

		uint16_t value = GetValue(AddMode::Direct, !(P & MFlag));

		EOR(value);

		return (!(P & MFlag) ? 4 : 3) + extraCycles;
	}
		// LSR dp
	case 0x46: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= DP << 8;

		int c = 0;
		if (DP & 0xFF) {
			c++;
		}

		uint16_t value = ReadMemory(add, true);

		if (!(P & MFlag)) {
			value |= ReadMemory(add + 1, true) << 8;
			c += 2;
		}

		uint16_t prev = value;

		bool M = P & MFlag;

		if (M) {
			uint8_t l = value & 0xFF;
			value &= 0xFF00;
			value |= l >> 1;
		}
		else {
			value >>= 1;
		}

		if ((M && ((value & 0xFF) == 0)) || (!M && (value == 0))) {
			P |= ZFlag;
		}
		else {
			P &= ~ZFlag;
		}

		if ((M && ((value & 0xFF) & 0x80)) || (!M && (value & 0x8000))) {
			P |= NFlag;
		}
		else {
			P &= ~NFlag;
		}

		if (prev & 0x1) {
			P |= CFlag;
		}
		else {
			P &= ~CFlag;
		}

		WriteMemory(add, value & 0xFF, true);

		if (!(P & MFlag)) {
			WriteMemory(add + 1, value >> 8, true);
		}

		return 5 + c;
	}
		// EOR [dp]
	case 0x47: {

		uint16_t value = GetValue(AddMode::DirectIndirectLong, !(P & MFlag));

		EOR(value);

		return (!(P & MFlag) ? 7 : 6) + extraCycles;
	}
		// PHA
	case 0x48:

		if (!(P & MFlag)) {
			Push((A & 0xFF00) >> 8);
		}
		Push(A & 0xFF);

		return !(P & MFlag) ? 4 : 3;

		// EOR #const
	case 0x49: {

		uint16_t value = GetValue(AddMode::Immediate, !(P & MFlag));

		EOR(value);

		return (!(P & MFlag) ? 3 : 2);
	}
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
	case 0x4C: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;

		PC = add;

		return 3;
	}
		// EOR addr
	case 0x4D: {

		uint16_t value = GetValue(AddMode::Absolute, !(P & MFlag));

		EOR(value);

		return (!(P & MFlag) ? 5 : 4);
	}
		// LSR addr
	case 0x4E: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;

		uint16_t value = ReadMemory(add, false);

		int c = 0;
		if (!(P & MFlag)) {
			value |= ReadMemory(add + 1, false) << 8;
			c += 2;
		}

		uint16_t prev = value;

		bool M = P & MFlag;

		if (M) {
			uint8_t l = value & 0xFF;
			value &= 0xFF00;
			value |= l >> 1;
		}
		else {
			value >>= 1;
		}

		if ((M && ((value & 0xFF) == 0)) || (!M && (value == 0))) {
			P |= ZFlag;
		}
		else {
			P &= ~ZFlag;
		}

		if ((M && ((value & 0xFF) & 0x80)) || (!M && (value & 0x8000))) {
			P |= NFlag;
		}
		else {
			P &= ~NFlag;
		}

		if (prev & 0x1) {
			P |= CFlag;
		}
		else {
			P &= ~CFlag;
		}

		WriteMemory(add, value & 0xFF, false);

		if (!(P & MFlag)) {
			WriteMemory(add + 1, value >> 8, false);
		}

		return 6 + c;
	}
		// EOR long
	case 0x4F: {

		uint16_t value = GetValue(AddMode::AbsoluteLong, !(P & MFlag));

		EOR(value);

		return (!(P & MFlag) ? 6 : 5);
	}
		// BVC nearlabel
	case 0x50: {

		int8_t offset = ReadMemory((PB << 16) | PC++, true);

		if (!(P & VFlag)) {

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
		// EOR (dp), Y
	case 0x51: {

		uint16_t value = GetValue(AddMode::DirectIndirectIndexed, !(P & MFlag));

		EOR(value);

		return (!(P & MFlag) ? 6 : 5) + extraCycles;
	}
		// EOR (dp)
	case 0x52: {

		uint16_t value = GetValue(AddMode::DirectIndirect, !(P & MFlag));

		EOR(value);

		return (!(P & MFlag) ? 6 : 5) + extraCycles;
	}
		// EOR (sr, S), Y
	case 0x53: {

		uint16_t value = GetValue(AddMode::StackRelativeIndirectIndexed, !(P & MFlag));

		EOR(value);

		return (!(P & MFlag) ? 8 : 7);
	}
		// MVN srcbk, destbk
	case 0x54: {

		if (!movActive) {
			srcbk = ReadMemory((PB << 16) | PC++, true);
			destbk = ReadMemory((PB << 16) | PC++, true);
			PC -= 2;
		}

		uint8_t value = ReadMemory((srcbk << 16) | X, true);
		WriteMemory((destbk << 16) | Y, value, true);

		A--;
		X++;
		Y++;

		if (A == 0xFFFF) {
			movActive = false;
			PC += 2;
		}
		else {
			movActive = true;
			PC--;
		}

		return 7;
	}
		// EOR dp, X
	case 0x55: {

		uint16_t value = GetValue(AddMode::DirectIndexedX, !(P & MFlag));

		EOR(value);

		return (!(P & MFlag) ? 5 : 4) + extraCycles;
	}
		// LSR dp, X
	case 0x56: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= DP << 8;
		add += X;

		int c = 0;
		if (DP & 0xFF) {
			c++;
		}

		uint16_t value = ReadMemory(add, true);

		if (!(P & MFlag)) {
			value |= ReadMemory(add + 1, true) << 8;
			c += 2;
		}

		uint16_t prev = value;

		bool M = P & MFlag;

		if (M) {
			uint8_t l = value & 0xFF;
			value &= 0xFF00;
			value |= l >> 1;
		}
		else {
			value >>= 1;
		}

		if ((M && ((value & 0xFF) == 0)) || (!M && (value == 0))) {
			P |= ZFlag;
		}
		else {
			P &= ~ZFlag;
		}

		if ((M && ((value & 0xFF) & 0x80)) || (!M && (value & 0x8000))) {
			P |= NFlag;
		}
		else {
			P &= ~NFlag;
		}

		if (prev & 0x1) {
			P |= CFlag;
		}
		else {
			P &= ~CFlag;
		}

		WriteMemory(add, value & 0xFF, true);

		if (!(P & MFlag)) {
			WriteMemory(add + 1, value >> 8, true);
		}

		return 6 + c;
	}
		// EOR [dp], Y
	case 0x57: {

		uint16_t value = GetValue(AddMode::DirectIndirectIndexedLong, !(P & MFlag));

		EOR(value);

		return (!(P & MFlag) ? 7 : 6) + extraCycles;
	}
		// CLI
	case 0x58:

		P &= ~IFlag;

		return 2;

		// EOR addr, Y
	case 0x59: {

		uint16_t value = GetValue(AddMode::AbsoluteIndexedY, !(P & MFlag));

		EOR(value);

		return (!(P & MFlag) ? 5 : 4) + extraCycles;
	}
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

		if (DP & 0x8000) {
			P |= NFlag;
		}
		else {
			P &= ~NFlag;
		}

		if (DP == 0) {
			P |= ZFlag;
		}
		else {
			P &= ~ZFlag;
		}

		return 2;

		// JMP long
	case 0x5C: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;
		PB = ReadMemory((PB << 16) | PC++, true);

		PC = add;

		return 4;
	}
		// EOR addr, X
	case 0x5D: {

		uint16_t value = GetValue(AddMode::AbsoluteIndexedX, !(P & MFlag));

		EOR(value);

		return (!(P & MFlag) ? 5 : 4) + extraCycles;
	}
		// LSR addr, X
	case 0x5E: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;
		add += X;

		uint16_t value = ReadMemory(add, false);

		int c = 0;
		if (!(P & MFlag)) {
			value |= ReadMemory(add + 1, false) << 8;
			c += 2;
		}

		uint16_t prev = value;

		bool M = P & MFlag;

		if (M) {
			uint8_t l = value & 0xFF;
			value &= 0xFF00;
			value |= l >> 1;
		}
		else {
			value >>= 1;
		}

		if ((M && ((value & 0xFF) == 0)) || (!M && (value == 0))) {
			P |= ZFlag;
		}
		else {
			P &= ~ZFlag;
		}

		if ((M && ((value & 0xFF) & 0x80)) || (!M && (value & 0x8000))) {
			P |= NFlag;
		}
		else {
			P &= ~NFlag;
		}

		if (prev & 0x1) {
			P |= CFlag;
		}
		else {
			P &= ~CFlag;
		}

		WriteMemory(add, value & 0xFF, false);

		if (!(P & MFlag)) {
			WriteMemory(add + 1, value >> 8, false);
		}

		return 7 + c;
	}
		// EOR long, X
	case 0x5F: {

		uint16_t value = GetValue(AddMode::AbsoluteIndexedLong, !(P & MFlag));

		EOR(value);

		return (!(P & MFlag) ? 6 : 5);
	}
		// RTS
	case 0x60: {

		PC = Pull();
		PC |= Pull() << 8;

		return 6;
	}
		// ADC (dp, X)
	case 0x61: {

		int16_t value = GetValue(AddMode::DirectIndexedIndirect, !(P & MFlag));

		ADC(value);

		return (!(P & MFlag) ? 7 : 6) + extraCycles;
	}
		// PER label
	case 0x62: {

		int16_t offset = ReadMemory((PB << 16) | PC++, true);
		offset |= ReadMemory((PB << 16) | PC++, true) << 8;

		offset += PC;

		Push(offset >> 8);
		Push(offset & 0xFF);

		return 6;
	}
		// ADC sr, S
	case 0x63: {

		int16_t value = GetValue(AddMode::StackRelative, !(P & MFlag));

		ADC(value);

		return !(P & MFlag) ? 5 : 4;
	}
		// STZ dp
	case 0x64: {

		uint32_t add = ReadMemory((PB << 16) | PC++, true);

		add |= DP << 8;

		WriteMemory(add, 0, true);

		int c = 0;

		if (!(P & MFlag)) {
			WriteMemory(add + 1, 0, true);
			c++;
		}

		if ((DP & 0xFF) != 0) {
			c++;
		}

		return 3 + c;
	}
		// ADC dp
	case 0x65: {

		int16_t value = GetValue(AddMode::Direct, !(P & MFlag));

		ADC(value);

		return (!(P & MFlag) ? 4 : 3) + extraCycles;
	}
		// ROR dp
	case 0x66: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= DP << 8;

		int c = 0;
		if (DP & 0xFF) {
			c++;
		}

		uint16_t value = ReadMemory(add, true);

		if (!(P & MFlag)) {
			value |= ReadMemory(add + 1, true) << 8;
			c += 2;
		}

		uint16_t prev = value;

		bool M = P & MFlag;

		if (M) {
			uint8_t l = value & 0xFF;
			value &= 0xFF00;
			value |= l >> 1;
			if (P & CFlag) {
				value |= 0x80;
			}
		}
		else {
			value >>= 1;
			if (P & CFlag) {
				value |= 0x8000;
			}
		}

		if ((M && ((value & 0xFF) == 0)) || (!M && (value == 0))) {
			P |= ZFlag;
		}
		else {
			P &= ~ZFlag;
		}

		if ((M && ((value & 0xFF) & 0x80)) || (!M && (value & 0x8000))) {
			P |= NFlag;
		}
		else {
			P &= ~NFlag;
		}

		if (prev & 0x1) {
			P |= CFlag;
		}
		else {
			P &= ~CFlag;
		}

		WriteMemory(add, value & 0xFF, true);

		if (!(P & MFlag)) {
			WriteMemory(add + 1, value >> 8, true);
		}

		return 5 + c;
	}
		// ADC [dp]
	case 0x67: {

		int16_t value = GetValue(AddMode::DirectIndirectLong, !(P & MFlag));

		ADC(value);

		return (!(P & MFlag) ? 7 : 6) + extraCycles;
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

		int16_t value = GetValue(AddMode::Immediate, !(P & MFlag));

		ADC(value);

		return !(P & MFlag) ? 3 : 2;
	}
		// ROR A
	case 0x6A: {

		uint16_t prevA = A;

		bool M = P & MFlag;

		if (M) {
			uint8_t l = A & 0xFF;
			A &= 0xFF00;
			A |= l >> 1;
			if (P & CFlag) {
				A |= 0x80;
			}
		}
		else {
			A >>= 1;
			if (P & CFlag) {
				A |= 0x8000;
			}
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
		// RTL
	case 0x6B: {

		PC = Pull();
		PC |= Pull() << 8;
		PB = Pull();

		return 6;
	}
		// JMP (addr)
	case 0x6C: {

		uint16_t indAdd = ReadMemory((PB << 16) | PC++, true);
		indAdd |= ReadMemory((PB << 16) | PC++, true) << 8;

		uint16_t add = ReadMemory(indAdd, false);
		add |= ReadMemory(indAdd + 1, false) << 8;

		PC = add;

		return 5;
	}
		// ADC addr
	case 0x6D: {

		int16_t value = GetValue(AddMode::Absolute, !(P & MFlag));

		ADC(value);

		return !(P & MFlag) ? 5 : 4;
	}
		// ROR addr
	case 0x6E: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;

		uint16_t value = ReadMemory(add, false);

		int c = 0;
		if (!(P & MFlag)) {
			value |= ReadMemory(add + 1, false) << 8;
			c += 2;
		}

		uint16_t prev = value;

		bool M = P & MFlag;

		if (M) {
			uint8_t l = value & 0xFF;
			value &= 0xFF00;
			value |= l >> 1;
			if (P & CFlag) {
				value |= 0x80;
			}
		}
		else {
			value >>= 1;
			if (P & CFlag) {
				value |= 0x8000;
			}
		}

		if ((M && ((value & 0xFF) == 0)) || (!M && (value == 0))) {
			P |= ZFlag;
		}
		else {
			P &= ~ZFlag;
		}

		if ((M && ((value & 0xFF) & 0x80)) || (!M && (value & 0x8000))) {
			P |= NFlag;
		}
		else {
			P &= ~NFlag;
		}

		if (prev & 0x1) {
			P |= CFlag;
		}
		else {
			P &= ~CFlag;
		}

		WriteMemory(add, value & 0xFF, false);

		if (!(P & MFlag)) {
			WriteMemory(add + 1, value >> 8, false);
		}

		return 6 + c;
	}
		// ADC long
	case 0x6F: {

		int16_t value = GetValue(AddMode::AbsoluteLong, !(P & MFlag));

		ADC(value);

		return !(P & MFlag) ? 6 : 5;
	}
		// BVS nearlabel
	case 0x70: {

		int8_t offset = ReadMemory((PB << 16) | PC++, true);

		if (P & VFlag) {

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
		// ADC (dp), Y
	case 0x71: {

		int16_t value = GetValue(AddMode::DirectIndirectIndexed, !(P & MFlag));

		ADC(value);

		return (!(P & MFlag) ? 6 : 5) + extraCycles;
	}
		// ADC (dp)
	case 0x72: {

		int16_t value = GetValue(AddMode::DirectIndirect, !(P & MFlag));

		ADC(value);

		return (!(P & MFlag) ? 6 : 5) + extraCycles;
	}
		// ADC (sr, S), Y
	case 0x73: {

		int16_t value = GetValue(AddMode::StackRelativeIndirectIndexed, !(P & MFlag));

		ADC(value);

		return !(P & MFlag) ? 8 : 7;
	}
		// STZ dp, X
	case 0x74: {

		uint32_t add = ReadMemory((PB << 16) | PC++, true);

		add |= DP << 8;
		add += X;

		WriteMemory(add, 0, true);

		int c = 0;

		if (!(P & MFlag)) {
			WriteMemory(add + 1, 0, true);
			c++;
		}

		if ((DP & 0xFF) != 0) {
			c++;
		}

		return 4 + c;
	}
		// ADC dp, X
	case 0x75: {

		int16_t value = GetValue(AddMode::DirectIndexedX, !(P & MFlag));

		ADC(value);

		return (!(P & MFlag) ? 5 : 4) + extraCycles;
	}
		// ROR dp, X
	case 0x76: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= DP << 8;
		add += X;

		int c = 0;
		if (DP & 0xFF) {
			c++;
		}

		uint16_t value = ReadMemory(add, true);

		if (!(P & MFlag)) {
			value |= ReadMemory(add + 1, true) << 8;
			c += 2;
		}

		uint16_t prev = value;

		bool M = P & MFlag;

		if (M) {
			uint8_t l = value & 0xFF;
			value &= 0xFF00;
			value |= l >> 1;
			if (P & CFlag) {
				value |= 0x80;
			}
		}
		else {
			value >>= 1;
			if (P & CFlag) {
				value |= 0x8000;
			}
		}

		if ((M && ((value & 0xFF) == 0)) || (!M && (value == 0))) {
			P |= ZFlag;
		}
		else {
			P &= ~ZFlag;
		}

		if ((M && ((value & 0xFF) & 0x80)) || (!M && (value & 0x8000))) {
			P |= NFlag;
		}
		else {
			P &= ~NFlag;
		}

		if (prev & 0x1) {
			P |= CFlag;
		}
		else {
			P &= ~CFlag;
		}

		WriteMemory(add, value & 0xFF, true);

		if (!(P & MFlag)) {
			WriteMemory(add + 1, value >> 8, true);
		}

		return 6 + c;
	}
		// ADC [dp], Y
	case 0x77: {

		int16_t value = GetValue(AddMode::DirectIndirectIndexedLong, !(P & MFlag));

		ADC(value);

		return (!(P & MFlag) ? 7 : 6) + extraCycles;
	}
		// SEI
	case 0x78:

		P |= IFlag;

		return 2;

		// ADC addr, Y
	case 0x79: {

		int16_t value = GetValue(AddMode::AbsoluteIndexedY, !(P & MFlag));

		ADC(value);

		return (!(P & MFlag) ? 5 : 4) + extraCycles;
	}
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

		A = DP;

		if (A & 0x8000) {
			P |= NFlag;
		}
		else {
			P &= ~NFlag;
		}

		if (A == 0) {
			P |= ZFlag;
		}
		else {
			P &= ~ZFlag;
		}

		return 2;

		// JMP (addr, X)
	case 0x7C: {

		uint16_t indAdd = ReadMemory((PB << 16) | PC++, true);
		indAdd |= ReadMemory((PB << 16) | PC++, true) << 8;
		indAdd += X;

		uint16_t add = ReadMemory(indAdd, false);
		add |= ReadMemory(indAdd + 1, false) << 8;

		PC = add;

		return 6;
	}
		// ADC addr, X
	case 0x7D: {

		int16_t value = GetValue(AddMode::AbsoluteIndexedX, !(P & MFlag));

		ADC(value);

		return (!(P & MFlag) ? 5 : 4) + extraCycles;
	}
		// ROR addr, X
	case 0x7E: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;
		add += X;

		uint16_t value = ReadMemory(add, false);

		int c = 0;
		if (!(P & MFlag)) {
			value |= ReadMemory(add + 1, false) << 8;
			c += 2;
		}

		uint16_t prev = value;

		bool M = P & MFlag;

		if (M) {
			uint8_t l = value & 0xFF;
			value &= 0xFF00;
			value |= l >> 1;
			if (P & CFlag) {
				value |= 0x80;
			}
		}
		else {
			value >>= 1;
			if (P & CFlag) {
				value |= 0x8000;
			}
		}

		if ((M && ((value & 0xFF) == 0)) || (!M && (value == 0))) {
			P |= ZFlag;
		}
		else {
			P &= ~ZFlag;
		}

		if ((M && ((value & 0xFF) & 0x80)) || (!M && (value & 0x8000))) {
			P |= NFlag;
		}
		else {
			P &= ~NFlag;
		}

		if (prev & 0x1) {
			P |= CFlag;
		}
		else {
			P &= ~CFlag;
		}

		WriteMemory(add, value & 0xFF, false);

		if (!(P & MFlag)) {
			WriteMemory(add + 1, value >> 8, false);
		}

		return 7 + c;
	}
		// ADC long, X
	case 0x7F: {

		int16_t value = GetValue(AddMode::AbsoluteIndexedLong, !(P & MFlag));

		ADC(value);

		return !(P & MFlag) ? 6 : 5;
	}
		// BRA nearlabel
	case 0x80: {

		int8_t offset = ReadMemory((PB << 16) | PC++, true);

		int c = 0;

		uint16_t page = PC & 0xFF00;
		PC += offset;
		if ((PC & 0xFF00) != page && emulationMode) {
			c = 1;
		}

		return 3 + c;
	}
		// STA (dp, X)
	case 0x81: {

		uint16_t indAdd = ReadMemory((PB << 16) | PC++, true);
		indAdd |= DP << 8;
		indAdd += X;

		int c = 0;
		if (DP & 0xFF) {
			c++;
		}

		uint16_t add = ReadMemory(indAdd, true);
		add |= ReadMemory(indAdd + 1, true) << 8;

		WriteMemory(add, A & 0xFF, true);

		if (!(P & MFlag)) {
			WriteMemory(add + 1, (A & 0xFF00) >> 8, true);
		}

		return (!(P & MFlag) ? 7 : 6) + c;
	}
		// BRL label
	case 0x82: {

		int16_t offset = ReadMemory((PB << 16) | PC++, true);
		offset |= ReadMemory((PB << 16) | PC++, true) << 8;

		PC += offset;

		return 4;
	}
		// STA sr, S
	case 0x83: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add += SP;

		WriteMemory(add, A & 0xFF, true);

		if (!(P & MFlag)) {
			WriteMemory(add + 1, A >> 8, true);
		}

		return !(P & MFlag) ? 5 : 4;
	}
		// STY dp
	case 0x84: {

		uint32_t add = ReadMemory((PB << 16) | PC++, true);

		add |= DP << 8;

		WriteMemory(add, Y & 0xFF, true);

		int c = 0;

		if (!(P & XFlag)) {
			WriteMemory(add + 1, Y >> 8, true);
			c++;
		}

		if ((DP & 0xFF) != 0) {
			c++;
		}

		return 3 + c;
	}
		// STA dp
	case 0x85: {

		uint32_t add = ReadMemory((PB << 16) | PC++, true);

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

		uint32_t add = ReadMemory((PB << 16) | PC++, true);

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
	case 0x87: {

		uint16_t indAdd = ReadMemory((PB << 16) | PC++, true);
		indAdd |= DP << 8;

		int c = 0;
		if (DP & 0xFF) {
			c++;
		}

		uint32_t add = ReadMemory(indAdd, true);
		add |= ReadMemory(indAdd + 1, true) << 8;
		add |= ReadMemory(indAdd + 2, true) << 16;

		WriteMemory(add, A & 0xFF, true);

		if (!(P & MFlag)) {
			WriteMemory(add + 1, A >> 8, true);
		}

		return (!(P & MFlag) ? 7 : 6) + c;
	}
		// DEY
	case 0x88:

		if (!(P & XFlag)) {
			Y--;
		}
		else {
			uint8_t l = Y & 0xFF;
			l--;
			Y &= 0xFF00;
			Y |= l;
		}

		CheckNFlag(Y, false, true);
		CheckZFlag(Y, false, true);

		return 2;

		// BIT #const
	case 0x89: {

		uint16_t value = GetValue(AddMode::Immediate, !(P & MFlag));
		uint16_t result;

		if (!(P & MFlag)) {
			result = A & value;
		}
		else {
			result = (A & 0xFF) & value;
		}

		CheckZFlag(result, true, false);

		return (!(P & MFlag) ? 3 : 2);
	}

		break;
		
		// TXA
	case 0x8A:

		if (!(P & MFlag)) {
			A = X;
		}
		else {
			A &= 0xFF00;
			A |= (X & 0xFF);
		}

		CheckNFlag(A, true, false);
		CheckZFlag(A, true, false);

		return 2;

		// PHB
	case 0x8B:

		Push(PB);

		return 3;

		// STY addr
	case 0x8C: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;

		WriteMemory(add, Y & 0xFF, false);

		if (!(P & XFlag)) {
			WriteMemory(add + 1, (Y & 0xFF00) >> 8, false);
		}

		return !(P & XFlag) ? 5 : 4;
	}
		// STA addr
	case 0x8D: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;

		WriteMemory(add, A & 0xFF, false);

		if (!(P & MFlag)) {
			WriteMemory(add + 1, (A & 0xFF00) >> 8, false);
		}

		return !(P & MFlag) ? 5 : 4;
	}
		// STX addr
	case 0x8E: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;

		WriteMemory(add, X & 0xFF, false);

		if (!(P & XFlag)) {
			WriteMemory(add + 1, (X & 0xFF00) >> 8, false);
		}

		return !(P & XFlag) ? 5 : 4;
	}
		// STA long
	case 0x8F: {

		uint32_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;
		add |= ReadMemory((PB << 16) | PC++, true) << 16;

		WriteMemory(add, A & 0xFF, true);

		if (!(P & MFlag)) {
			WriteMemory(add + 1, A >> 8, true);
		}

		return !(P & MFlag) ? 6 : 5;
	}
		// BCC nearlabel
	case 0x90: {

		int8_t offset = ReadMemory((PB << 16) | PC++, true);

		if (!(P & CFlag)) {

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
		// STA (dp), Y
	case 0x91: {

		uint16_t indAdd = ReadMemory((PB << 16) | PC++, true);
		indAdd |= DP << 8;

		int c = 0;
		if (DP & 0xFF) {
			c++;
		}

		uint16_t add = ReadMemory(indAdd, true);
		add |= ReadMemory(indAdd + 1, true) << 8;

		add += Y;

		WriteMemory(add, A & 0xFF, true);

		if (!(P & MFlag)) {
			WriteMemory(add + 1, A >> 8, true);
		}

		return (!(P & MFlag) ? 7 : 6) + c;
	}
		// STA (dp)
	case 0x92: {

		uint16_t indAdd = ReadMemory((PB << 16) | PC++, true);
		indAdd |= DP << 8;

		int c = 0;
		if (DP & 0xFF) {
			c++;
		}

		uint16_t add = ReadMemory(indAdd, true);
		add |= ReadMemory(indAdd + 1, true) << 8;

		WriteMemory(add, A & 0xFF, true);

		if (!(P & MFlag)) {
			WriteMemory(add + 1, A >> 8, true);
		}

		return (!(P & MFlag) ? 6 : 5) + c;
	}
		// STA (sr, S), Y
	case 0x93: {

		uint16_t indAdd = ReadMemory((PB << 16) | PC++, true);
		indAdd += SP;

		uint16_t add = ReadMemory(indAdd, true);
		add |= ReadMemory(indAdd + 1, true) << 8;
		add += Y;

		WriteMemory(add, A & 0xFF, true);

		if (!(P & MFlag)) {
			WriteMemory(add + 1, A >> 8, true);
		}

		return !(P & MFlag) ? 8 : 7;
	}
		// STY dp, X
	case 0x94: {

		uint32_t add = ReadMemory((PB << 16) | PC++, true);

		add |= DP << 8;
		add += X;

		WriteMemory(add, Y & 0xFF, true);

		int c = 0;

		if (!(P & XFlag)) {
			WriteMemory(add + 1, Y >> 8, true);
			c++;
		}

		if ((DP & 0xFF) != 0) {
			c++;
		}

		return 4 + c;
	}

		break;

		// STA dp, X
	case 0x95: {

		uint32_t add = ReadMemory((PB << 16) | PC++, true);

		add |= DP << 8;
		add += X;

		WriteMemory(add, A & 0xFF, true);

		int c = 0;

		if (!(P & MFlag)) {
			WriteMemory(add + 1, A >> 8, true);
			c++;
		}

		if ((DP & 0xFF) != 0) {
			c++;
		}

		return 4 + c;
	}
		// STX dp, Y
	case 0x96: {

		uint32_t add = ReadMemory((PB << 16) | PC++, true);

		add |= DP << 8;
		add += Y;

		WriteMemory(add, X & 0xFF, true);

		int c = 0;

		if (!(P & XFlag)) {
			WriteMemory(add + 1, X >> 8, true);
			c++;
		}

		if ((DP & 0xFF) != 0) {
			c++;
		}

		return 4 + c;
	}
		// STA [dp], Y
	case 0x97: {

		uint16_t indAdd = ReadMemory((PB << 16) | PC++, true);
		indAdd |= DP << 8;

		int c = 0;
		if (DP & 0xFF) {
			c++;
		}

		uint32_t add = ReadMemory(indAdd, true);
		add |= ReadMemory(indAdd + 1, true) << 8;
		add |= ReadMemory(indAdd + 2, true) << 16;
		add += Y;

		WriteMemory(add, A & 0xFF, true);

		if (!(P & MFlag)) {
			WriteMemory(add + 1, A >> 8, true);
		}

		return (!(P & MFlag) ? 7 : 6) + c;
	}
		// TYA
	case 0x98:

		if (!(P & MFlag)) {
			A = Y;
		}
		else {
			A &= 0xFF00;
			A |= (Y & 0xFF);
		}

		CheckNFlag(A, true, false);
		CheckZFlag(A, true, false);

		return 2;

		// STA addr, Y
	case 0x99: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;
		add += Y;

		WriteMemory(add, A & 0xFF, false);

		if (!(P & MFlag)) {
			WriteMemory(add + 1, (A & 0xFF00) >> 8, false);
		}

		return !(P & MFlag) ? 6 : 5;
	}
		// TXS
	case 0x9A:

		SP = X;

		return 2;

		// TXY
	case 0x9B:

		if (!(P & XFlag)) {
			Y = X;
		}
		else {
			Y &= 0xFF00;
			Y |= (X & 0xFF);
		}

		CheckNFlag(X, false, true);
		CheckZFlag(X, false, true);

		return 2;

		// STZ addr
	case 0x9C: {

		uint16_t dir = ReadMemory((PB << 16) | PC++, true);
		dir |= ReadMemory((PB << 16) | PC++, true) << 8;

		WriteMemory(dir, 0, false);

		if (!(P & MFlag)) {
			WriteMemory(dir + 1, 0, false);
		}

		return !(P & MFlag) ? 5 : 4;
	}
		// STA addr, X
	case 0x9D: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;
		add += X;

		WriteMemory(add, A & 0xFF, false);

		if (!(P & MFlag)) {
			WriteMemory(add + 1, (A & 0xFF00) >> 8, false);
		}

		return !(P & MFlag) ? 6 : 5;
	}
		// STZ addr, X
	case 0x9E: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;
		add += X;

		WriteMemory(add, 0, false);

		if (!(P & MFlag)) {
			WriteMemory(add + 1, 0, false);
		}

		return !(P & MFlag) ? 6 : 5;
	}
		// STA long, X
	case 0x9F: {

		uint32_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;
		add |= ReadMemory((PB << 16) | PC++, true) << 16;

		add += X;

		WriteMemory(add, A & 0xFF, true);

		if (!(P & MFlag)) {
			WriteMemory(add + 1, A >> 8, true);
		}

		return !(P & MFlag) ? 6 : 5;
	}
		// LDY #const
	case 0xA0: {

		if (!(P & XFlag)) {
			Y = GetValue(AddMode::Immediate, !(P & XFlag));
		}
		else {
			uint8_t value = (uint8_t)GetValue(AddMode::Immediate, !(P & XFlag));
			Y &= 0xFF00;
			Y |= value;
		}

		CheckNFlag(Y, false, true);
		CheckZFlag(Y, false, true);

		return !(P & XFlag) ? 3 : 2;
	}
		// LDA (dp, X)
	case 0xA1:

		if (!(P & MFlag)) {
			A = GetValue(AddMode::DirectIndexedIndirect, !(P & MFlag));
		}
		else {
			uint8_t value = (uint8_t)GetValue(AddMode::DirectIndexedIndirect, !(P & MFlag));
			A &= 0xFF00;
			A |= value;
		}

		CheckNFlag(A, true, false);
		CheckZFlag(A, true, false);

		return (!(P & MFlag) ? 7 : 6) + extraCycles;

		// LDX #const
	case 0xA2:

		if (!(P & XFlag)) {
			X = GetValue(AddMode::Immediate, !(P & XFlag));
		}
		else {
			uint8_t value = (uint8_t)GetValue(AddMode::Immediate, !(P & XFlag));
			X &= 0xFF00;
			X |= value;
		}

		CheckNFlag(X, false, true);
		CheckZFlag(X, false, true);

		return !(P & XFlag) ? 3 : 2;

		// LDA sr, S
	case 0xA3:

		if (!(P & MFlag)) {
			A = GetValue(AddMode::StackRelative, !(P & MFlag));
		}
		else {
			uint8_t value = (uint8_t)GetValue(AddMode::StackRelative, !(P & MFlag));
			A &= 0xFF00;
			A |= value;
		}

		CheckNFlag(A, true, false);
		CheckZFlag(A, true, false);

		return (!(P & MFlag) ? 5 : 4);

		// LDY dp
	case 0xA4:

		if (!(P & XFlag)) {
			Y = GetValue(AddMode::Direct, !(P & XFlag));
		}
		else {
			uint8_t value = (uint8_t)GetValue(AddMode::Direct, !(P & XFlag));
			Y &= 0xFF00;
			Y |= value;
		}

		CheckNFlag(Y, false, true);
		CheckZFlag(Y, false, true);

		return (!(P & XFlag) ? 4 : 3) + extraCycles;

		// LDA dp
	case 0xA5:

		if (!(P & MFlag)) {
			A = GetValue(AddMode::Direct, !(P & MFlag));
		}
		else {
			uint8_t value = (uint8_t)GetValue(AddMode::Direct, !(P & MFlag));
			A &= 0xFF00;
			A |= value;
		}

		CheckNFlag(A, true, false);
		CheckZFlag(A, true, false);

		return (!(P & MFlag) ? 4 : 3) + extraCycles;

		// LDX dp
	case 0xA6: {

		if (!(P & XFlag)) {
			X = GetValue(AddMode::Direct, !(P & XFlag));
		}
		else {
			uint8_t value = (uint8_t)GetValue(AddMode::Direct, !(P & XFlag));
			X &= 0xFF00;
			X |= value;
		}

		CheckNFlag(X, false, true);
		CheckZFlag(X, false, true);

		return (!(P & MFlag) ? 4 : 3) + extraCycles;
	}
		// LDA [dp]
	case 0xA7:

		if (!(P & MFlag)) {
			A = GetValue(AddMode::DirectIndirectLong, !(P & MFlag));
		}
		else {
			uint8_t value = (uint8_t)GetValue(AddMode::DirectIndirectLong, !(P & MFlag));
			A &= 0xFF00;
			A |= value;
		}

		CheckNFlag(A, true, false);
		CheckZFlag(A, true, false);

		return (!(P & MFlag) ? 7 : 6) + extraCycles;

		// TAY
	case 0xA8:

		if (!(P & XFlag)) {
			Y = A;
		}
		else {
			Y &= 0xFF00;
			Y |= (A & 0xFF);
		}

		CheckNFlag(Y, false, true);
		CheckZFlag(Y, false, true);

		return 2;

		// LDA #const
	case 0xA9: {

		if (!(P & MFlag)) {
			A = GetValue(AddMode::Immediate, !(P & MFlag));
		}
		else {
			uint8_t value = (uint8_t)GetValue(AddMode::Immediate, !(P & MFlag));
			A &= 0xFF00;
			A |= value;
		}

		CheckNFlag(A, true, false);
		CheckZFlag(A, true, false);

		return !(P & MFlag) ? 3 : 2;
	}
		// TAX
	case 0xAA:

		if (!(P & XFlag)) {
			X = A;
		}
		else {
			X &= 0xFF00;
			X |= (A & 0xFF);
		}

		CheckNFlag(X, false, true);
		CheckZFlag(X, false, true);

		return 2;

		// PLB
	case 0xAB:

		PB = Pull();

		CheckNFlag(PB, false, false);
		CheckZFlag(PB, false, false);

		return 4;

		// LDY addr
	case 0xAC:

		if (!(P & XFlag)) {
			Y = GetValue(AddMode::Absolute, !(P & XFlag));
		}
		else {
			uint8_t value = (uint8_t)GetValue(AddMode::Absolute, !(P & XFlag));
			Y &= 0xFF00;
			Y |= value;
		}

		CheckNFlag(Y, false, true);
		CheckZFlag(Y, false, true);

		return !(P & XFlag) ? 5 : 4;

		// LDA addr
	case 0xAD:

		if (!(P & MFlag)) {
			A = GetValue(AddMode::Absolute, !(P & MFlag));
		}
		else {
			uint8_t value = (uint8_t)GetValue(AddMode::Absolute, !(P & MFlag));
			A &= 0xFF00;
			A |= value;
		}

		CheckNFlag(A, true, false);
		CheckZFlag(A, true, false);

		return !(P & MFlag) ? 5 : 4;

		// LDX addr
	case 0xAE:

		if (!(P & XFlag)) {
			X = GetValue(AddMode::Absolute, !(P & XFlag));
		}
		else {
			uint8_t value = (uint8_t)GetValue(AddMode::Absolute, !(P & XFlag));
			X &= 0xFF00;
			X |= value;
		}

		CheckNFlag(X, false, true);
		CheckZFlag(X, false, true);

		return !(P & XFlag) ? 5 : 4;

		// LDA long
	case 0xAF:

		if (!(P & MFlag)) {
			A = GetValue(AddMode::AbsoluteLong, !(P & MFlag));
		}
		else {
			uint8_t value = (uint8_t)GetValue(AddMode::AbsoluteLong, !(P & MFlag));
			A &= 0xFF00;
			A |= value;
		}

		CheckNFlag(A, true, false);
		CheckZFlag(A, true, false);

		return !(P & MFlag) ? 6 : 5;

		// BCS nearlabel
	case 0xB0: {

		int8_t offset = ReadMemory((PB << 16) | PC++, true);

		if (P & CFlag) {

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
		// LDA (dp), Y
	case 0xB1:

		if (!(P & MFlag)) {
			A = GetValue(AddMode::DirectIndirectIndexed, !(P & MFlag));
		}
		else {
			uint8_t value = (uint8_t)GetValue(AddMode::DirectIndirectIndexed, !(P & MFlag));
			A &= 0xFF00;
			A |= value;
		}

		CheckNFlag(A, true, false);
		CheckZFlag(A, true, false);

		return (!(P & MFlag) ? 6 : 5) + extraCycles;

		// LDA (dp)
	case 0xB2:

		if (!(P & MFlag)) {
			A = GetValue(AddMode::DirectIndirect, !(P & MFlag));
		}
		else {
			uint8_t value = (uint8_t)GetValue(AddMode::DirectIndirect, !(P & MFlag));
			A &= 0xFF00;
			A |= value;
		}

		CheckNFlag(A, true, false);
		CheckZFlag(A, true, false);

		return (!(P & MFlag) ? 6 : 5) + extraCycles;

		// LDA (sr, S), Y
	case 0xB3:

		if (!(P & MFlag)) {
			A = GetValue(AddMode::StackRelativeIndirectIndexed, !(P & MFlag));
		}
		else {
			uint8_t value = (uint8_t)GetValue(AddMode::StackRelativeIndirectIndexed, !(P & MFlag));
			A &= 0xFF00;
			A |= value;
		}

		CheckNFlag(A, true, false);
		CheckZFlag(A, true, false);

		return !(P & MFlag) ? 8 : 7;

		// LDY dp, X
	case 0xB4:

		if (!(P & XFlag)) {
			Y = GetValue(AddMode::DirectIndexedX, !(P & XFlag));
		}
		else {
			uint8_t value = (uint8_t)GetValue(AddMode::DirectIndexedX, !(P & XFlag));
			Y &= 0xFF00;
			Y |= value;
		}

		CheckNFlag(Y, false, true);
		CheckZFlag(Y, false, true);

		return (!(P & XFlag) ? 5 : 4) + extraCycles;

		// LDA dp, X
	case 0xB5:

		if (!(P & MFlag)) {
			A = GetValue(AddMode::DirectIndexedX, !(P & MFlag));
		}
		else {
			uint8_t value = (uint8_t)GetValue(AddMode::DirectIndexedX, !(P & MFlag));
			A &= 0xFF00;
			A |= value;
		}

		CheckNFlag(A, true, false);
		CheckZFlag(A, true, false);

		return (!(P & MFlag) ? 5 : 4) + extraCycles;

		// LDX dp, Y
	case 0xB6:

		if (!(P & XFlag)) {
			X = GetValue(AddMode::DirectIndexedY, !(P & XFlag));
		}
		else {
			uint8_t value = (uint8_t)GetValue(AddMode::DirectIndexedY, !(P & XFlag));
			X &= 0xFF00;
			X |= value;
		}

		CheckNFlag(X, false, true);
		CheckZFlag(X, false, true);

		return (!(P & XFlag) ? 5 : 4) + extraCycles;

		// LDA [dp], Y
	case 0xB7:

		if (!(P & MFlag)) {
			A = GetValue(AddMode::DirectIndirectIndexedLong, !(P & MFlag));
		}
		else {
			uint8_t value = (uint8_t)GetValue(AddMode::DirectIndirectIndexedLong, !(P & MFlag));
			A &= 0xFF00;
			A |= value;
		}

		CheckNFlag(A, true, false);
		CheckZFlag(A, true, false);

		return (!(P & MFlag) ? 7 : 6) + extraCycles;

		// CLV
	case 0xB8:

		P &= ~VFlag;

		return 2;

		// LDA addr, Y
	case 0xB9:

		if (!(P & MFlag)) {
			A = GetValue(AddMode::AbsoluteIndexedY, !(P & MFlag));
		}
		else {
			uint8_t value = (uint8_t)GetValue(AddMode::AbsoluteIndexedY, !(P & MFlag));
			A &= 0xFF00;
			A |= value;
		}

		CheckNFlag(A, true, false);
		CheckZFlag(A, true, false);

		return (!(P & MFlag) ? 5 : 4) + extraCycles;

		// TSX
	case 0xBA:

		if (!(P & XFlag)) {
			X = SP;
		}
		else {
			X &= 0xFF00;
			X |= (SP & 0xFF);
		}

		CheckNFlag(X, false, true);
		CheckZFlag(X, false, true);

		return 2;

		// TYX
	case 0xBB:

		if (!(P & XFlag)) {
			X = Y;
		}
		else {
			X &= 0xFF00;
			X |= (Y & 0xFF);
		}

		CheckNFlag(X, false, true);
		CheckZFlag(X, false, true);

		return 2;

		// LDY addr, X
	case 0xBC:

		if (!(P & XFlag)) {
			Y = GetValue(AddMode::AbsoluteIndexedX, !(P & XFlag));
		}
		else {
			uint8_t value = (uint8_t)GetValue(AddMode::AbsoluteIndexedX, !(P & XFlag));
			Y &= 0xFF00;
			Y |= value;
		}

		CheckNFlag(Y, false, true);
		CheckZFlag(Y, false, true);

		return (!(P & XFlag) ? 5 : 4) + extraCycles;

		// LDA addr, X
	case 0xBD:

		if (!(P & MFlag)) {
			A = GetValue(AddMode::AbsoluteIndexedX, !(P & MFlag));
		}
		else {
			uint8_t value = (uint8_t)GetValue(AddMode::AbsoluteIndexedX, !(P & MFlag));
			A &= 0xFF00;
			A |= value;
		}

		CheckNFlag(A, true, false);
		CheckZFlag(A, true, false);

		return (!(P & MFlag) ? 5 : 4) + extraCycles;

		// LDX addr, Y
	case 0xBE:

		if (!(P & XFlag)) {
			X = GetValue(AddMode::AbsoluteIndexedY, !(P & XFlag));
		}
		else {
			uint8_t value = (uint8_t)GetValue(AddMode::AbsoluteIndexedY, !(P & XFlag));
			X &= 0xFF00;
			X |= value;
		}

		CheckNFlag(X, false, true);
		CheckZFlag(X, false, true);

		return (!(P & XFlag) ? 5 : 4) + extraCycles;

		// LDA long, X
	case 0xBF:

		if (!(P & MFlag)) {
			A = GetValue(AddMode::AbsoluteIndexedLong, !(P & MFlag));
		}
		else {
			uint8_t value = (uint8_t)GetValue(AddMode::AbsoluteIndexedLong, !(P & MFlag));
			A &= 0xFF00;
			A |= value;
		}

		CheckNFlag(A, true, false);
		CheckZFlag(A, true, false);

		return !(P & MFlag) ? 6 : 5;

		// CPY #const
	case 0xC0: {

		uint16_t value = GetValue(AddMode::Immediate, !(P & XFlag));

		CPY(value);

		return (!(P & XFlag) ? 3 : 2);
	}
		// CMP (dp, X)
	case 0xC1: {

		uint16_t value = GetValue(AddMode::DirectIndexedIndirect, !(P & MFlag));

		CMP(value);

		return (!(P & MFlag) ? 7 : 6 ) + extraCycles;
	}
		// REP #const
	case 0xC2: {

		uint8_t value = (uint8_t)GetValue(AddMode::Immediate, false);
		P &= ~value;

		if (P & XFlag) {
			X &= 0xFF;
			Y &= 0xFF;
		}

		return 3;
	}

		// CMP sr, S
	case 0xC3: {

		uint16_t value = GetValue(AddMode::StackRelative, !(P & MFlag));

		CMP(value);

		return (!(P & MFlag) ? 5 : 4);
	}
		// CPY dp
	case 0xC4: {

		uint16_t value = GetValue(AddMode::Direct, !(P & XFlag));

		CPY(value);

		return (!(P & XFlag) ? 4 : 3) + extraCycles;
	}
		// CMP dp
	case 0xC5: {

		uint16_t value = GetValue(AddMode::Direct, !(P & MFlag));

		CMP(value);

		return (!(P & MFlag) ? 4 : 3) + extraCycles;
	}
		// DEC dp
	case 0xC6: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= DP << 8;

		int c = 0;
		if (DP & 0xFF) {
			c++;
		}

		uint16_t value = ReadMemory(add, true);

		if (!(P & MFlag)) {
			value |= ReadMemory(add + 1, true) << 8;
			c += 2;
			value--;
		}
		else {
			uint8_t l = value & 0xFF;
			l--;
			value &= 0xFF00;
			value |= l;
		}

		CheckNFlag(value, true, false);
		CheckZFlag(value, true, false);

		WriteMemory(add, (value & 0xFF), true);
		if (!(P & MFlag)) {
			WriteMemory(add + 1, value >> 8, true);
		}

		return 5 + c;
	}
		// CMP [dp]
	case 0xC7: {

		uint16_t value = GetValue(AddMode::DirectIndirectLong, !(P & MFlag));

		CMP(value);

		return (!(P & MFlag) ? 7 : 6) + extraCycles;
	}
		// INY
	case 0xC8:

		if (!(P & XFlag)) {
			Y++;
		}
		else {
			uint8_t l = Y & 0xFF;
			l++;
			Y &= 0xFF00;
			Y |= l;
		}

		CheckNFlag(Y, false, true);
		CheckZFlag(Y, false, true);

		return 2;

		// CMP #const
	case 0xC9: {

		uint16_t value = GetValue(AddMode::Immediate, !(P & MFlag));

		CMP(value);

		return !(P & MFlag) ? 3 : 2;
	}
		// DEX
	case 0xCA:

		if (!(P & XFlag)) {
			X--;
		}
		else {
			uint8_t l = X & 0xFF;
			l--;
			X &= 0xFF00;
			X |= l;
		}

		CheckNFlag(X, false, true);
		CheckZFlag(X, false, true);

		return 2;

		// WAI
	case 0xCB:

		break;

		// CPY addr
	case 0xCC: {

		uint16_t value = GetValue(AddMode::Absolute, !(P & XFlag));

		CPY(value);

		return (!(P & XFlag) ? 5 : 4);
	}
		// CMP addr
	case 0xCD: {

		uint16_t value = GetValue(AddMode::Absolute, !(P & MFlag));

		CMP(value);

		return !(P & MFlag) ? 5 : 4;
	}
		// DEC addr
	case 0xCE: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;

		uint16_t value = ReadMemory(add, false);

		int c = 0;
		if (!(P & MFlag)) {
			value |= ReadMemory(add + 1, false) << 8;
			c += 2;
			value--;
		}
		else {
			uint8_t l = value & 0xFF;
			l--;
			value &= 0xFF00;
			value |= l;
		}

		CheckNFlag(value, true, false);
		CheckZFlag(value, true, false);

		WriteMemory(add, (value & 0xFF), false);
		if (!(P & MFlag)) {
			WriteMemory(add + 1, value >> 8, false);
		}

		return 6 + c;
	}
		// CMP long
	case 0xCF: {

		uint16_t value = GetValue(AddMode::AbsoluteLong, !(P & MFlag));

		CMP(value);

		return (!(P & MFlag) ? 6 : 5);
	}
		// BNE nearlabel
	case 0xD0: {

		int8_t offset = ReadMemory((PB << 16) | PC++, true);

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
	case 0xD1: {

		uint16_t value = GetValue(AddMode::DirectIndirectIndexed, !(P & MFlag));

		CMP(value);

		return (!(P & MFlag) ? 6 : 5) + extraCycles;
	}
		// CMP (dp)
	case 0xD2: {

		uint16_t value = GetValue(AddMode::DirectIndirect, !(P & MFlag));

		CMP(value);

		return (!(P & MFlag) ? 6 : 5) + extraCycles;
	}
		// CMP (sr, S), Y
	case 0xD3: {

		uint16_t value = GetValue(AddMode::StackRelativeIndirectIndexed, !(P & MFlag));

		CMP(value);

		return (!(P & MFlag) ? 8 : 7);
	}
		// PEI (dp)
	case 0xD4: {

		uint16_t indAdd = ReadMemory((PB << 16) | PC++, true);
		indAdd |= DP << 8;

		int c = 0;
		if (DP & 0xFF) {
			c++;
		}

		uint16_t add = ReadMemory(indAdd, true);
		add |= ReadMemory(indAdd + 1, true) << 8;

		Push(add >> 8);
		Push(add & 0xFF);

		return 6 + c;
	}
		// CMP dp, X
	case 0xD5: {

		uint16_t value = GetValue(AddMode::DirectIndexedX, !(P & MFlag));

		CMP(value);

		return (!(P & MFlag) ? 5 : 4) + extraCycles;
	}
		// DEC dp, X
	case 0xD6: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= DP << 8;
		add += X;

		int c = 0;
		if (DP & 0xFF) {
			c++;
		}

		uint16_t value = ReadMemory(add, true);

		if (!(P & MFlag)) {
			value |= ReadMemory(add + 1, true) << 8;
			c += 2;
			value--;
		}
		else {
			uint8_t l = value & 0xFF;
			l--;
			value &= 0xFF00;
			value |= l;
		}

		CheckNFlag(value, true, false);
		CheckZFlag(value, true, false);

		WriteMemory(add, (value & 0xFF), true);
		if (!(P & MFlag)) {
			WriteMemory(add + 1, value >> 8, true);
		}

		return 6 + c;
	}
		// CMP [dp], Y
	case 0xD7: {

		uint16_t value = GetValue(AddMode::DirectIndirectIndexedLong, !(P & MFlag));

		CMP(value);

		return (!(P & MFlag) ? 7 : 6) + extraCycles;
	}
		// CLD
	case 0xD8:

		P &= ~DFlag;

		return 2;

		// CMP addr, Y
	case 0xD9: {

		uint16_t value = GetValue(AddMode::AbsoluteIndexedY, !(P & MFlag));

		CMP(value);

		return (!(P & MFlag) ? 5 : 4) + extraCycles;
	}
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
	case 0xDC: {

		uint16_t indAdd = ReadMemory((PB << 16) | PC++, true);
		indAdd |= ReadMemory((PB << 16) | PC++, true) << 8;

		uint32_t add = ReadMemory(indAdd, false);
		add |= ReadMemory(indAdd + 1, false) << 8;
		PB = ReadMemory(indAdd + 2, false);

		PC = add;

		return 6;
	}
		// CMP addr, X
	case 0xDD: {

		uint16_t value = GetValue(AddMode::AbsoluteIndexedX, !(P & MFlag));

		CMP(value);

		return (!(P & MFlag) ? 5 : 4) + extraCycles;
	}
		// DEC addr, X
	case 0xDE: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;
		add += X;

		uint16_t value = ReadMemory(add, false);

		int c = 0;
		if (!(P & MFlag)) {
			value |= ReadMemory(add + 1, false) << 8;
			c += 2;
			value--;
		}
		else {
			uint8_t l = value & 0xFF;
			l--;
			value &= 0xFF00;
			value |= l;
		}

		CheckNFlag(value, true, false);
		CheckZFlag(value, true, false);

		WriteMemory(add, (value & 0xFF), false);
		if (!(P & MFlag)) {
			WriteMemory(add + 1, value >> 8, false);
		}

		return 7 + c;
	}
		// CMP long, X
	case 0xDF: {

		uint16_t value = GetValue(AddMode::AbsoluteIndexedLong, !(P & MFlag));

		CMP(value);

		return (!(P & MFlag) ? 6 : 5);
	}
		// CPX #const
	case 0xE0: {

		uint16_t value = GetValue(AddMode::Immediate, !(P & XFlag));

		CPX(value);

		return !(P & XFlag) ? 3 : 2;
	}
		// SBC (dp, X)
	case 0xE1: {

		int16_t value = GetValue(AddMode::DirectIndexedIndirect, !(P & MFlag));

		SBC(value);

		return (!(P & MFlag) ? 7 : 6) + extraCycles;
	}
		// SEP #const
	case 0xE2: {

		uint8_t value = (uint8_t)GetValue(AddMode::Immediate, false);
		P |= value;

		if (P & XFlag) {
			X &= 0xFF;
			Y &= 0xFF;
		}

		return 3;
	}

		// SBC sr, S
	case 0xE3: {

		int16_t value = GetValue(AddMode::StackRelative, !(P & MFlag));

		SBC(value);

		return !(P & MFlag) ? 5 : 4;
	}
		// CPX dp
	case 0xE4: {

		uint16_t value = GetValue(AddMode::Direct, !(P & XFlag));

		CPX(value);

		return (!(P & XFlag) ? 4 : 3) + extraCycles;
	}
		// SBC dp
	case 0xE5: {

		int16_t value = GetValue(AddMode::Direct, !(P & MFlag));

		SBC(value);

		return (!(P & MFlag) ? 4 : 3) + extraCycles;
	}
		// INC dp
	case 0xE6: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= DP << 8;

		int c = 0;
		if (DP & 0xFF) {
			c++;
		}

		uint16_t value = ReadMemory(add, true);

		if (!(P & MFlag)) {
			value |= ReadMemory(add + 1, true) << 8;
			c += 2;
			value++;
		}
		else {
			uint8_t l = value & 0xFF;
			l++;
			value &= 0xFF00;
			value |= l;
		}

		CheckNFlag(value, true, false);
		CheckZFlag(value, true, false);

		WriteMemory(add, (value & 0xFF), true);
		if (!(P & MFlag)) {
			WriteMemory(add + 1, value >> 8, true);
		}

		return 5 + c;
	}
		// SBC [dp]
	case 0xE7: {

		int16_t value = GetValue(AddMode::DirectIndirectLong, !(P & MFlag));

		SBC(value);

		return (!(P & MFlag) ? 7 : 6) + extraCycles;
	}
		// INX
	case 0xE8:

		if (!(P & XFlag)) {
			X++;
		}
		else {
			uint8_t l = X & 0xFF;
			l++;
			X &= 0xFF00;
			X |= l;
		}

		CheckNFlag(X, false, true);
		CheckZFlag(X, false, true);

		return 2;

		// SBC #const
	case 0xE9: {

		int16_t value = GetValue(AddMode::Immediate, !(P & MFlag));

		SBC(value);

		return !(P & MFlag) ? 3 : 2;
	}
		// NOP
	case 0xEA:

		return 2;

		// XBA
	case 0xEB: {

		uint16_t low = A & 0xFF;
		uint8_t high = (A & 0xFF00) >> 8;

		A = high | (low << 8);

		if (A & 0x80) {
			P |= NFlag;
		}
		else {
			P &= ~NFlag;
		}

		if ((A & 0xFF) == 0) {
			P |= ZFlag;
		}
		else {
			P &= ~ZFlag;
		}

		return 3;
	}

		// CPX addr
	case 0xEC: {

		uint16_t value = GetValue(AddMode::Absolute, !(P & XFlag));

		CPX(value);

		return !(P & XFlag) ? 5 : 4;
	}
		// SBC addr
	case 0xED: {

		int16_t value = GetValue(AddMode::Absolute, !(P & MFlag));

		SBC(value);

		return !(P & MFlag) ? 5 : 4;
	}
		// INC addr
	case 0xEE: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;

		uint16_t value = ReadMemory(add, false);

		int c = 0;
		if (!(P & MFlag)) {
			value |= ReadMemory(add + 1, false) << 8;
			c += 2;
			value++;
		}
		else {
			uint8_t l = value & 0xFF;
			l++;
			value &= 0xFF00;
			value |= l;
		}

		CheckNFlag(value, true, false);
		CheckZFlag(value, true, false);

		WriteMemory(add, (value & 0xFF), false);
		if (!(P & MFlag)) {
			WriteMemory(add + 1, value >> 8, false);
		}

		return 6 + c;
	}
		// SBC long
	case 0xEF: {

		int16_t value = GetValue(AddMode::AbsoluteLong, !(P & MFlag));

		SBC(value);

		return !(P & MFlag) ? 6 : 5;
	}
		// BEQ nearlabel
	case 0xF0: {

		int8_t offset = ReadMemory((PB << 16) | PC++, true);

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
	case 0xF1: {

		int16_t value = GetValue(AddMode::DirectIndirectIndexed, !(P & MFlag));

		SBC(value);

		return (!(P & MFlag) ? 6 : 5) + extraCycles;
	}
		// SBC (dp)
	case 0xF2: {

		int16_t value = GetValue(AddMode::DirectIndirect, !(P & MFlag));

		SBC(value);

		return (!(P & MFlag) ? 6 : 5) + extraCycles;
	}
		// SBC (sr, S), Y
	case 0xF3: {

		int16_t value = GetValue(AddMode::StackRelativeIndirectIndexed, !(P & MFlag));

		SBC(value);

		return !(P & MFlag) ? 8 : 7;
	}
		// PEA addr
	case 0xF4: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;

		Push(add >> 8);
		Push(add & 0xFF);

		return 5;
	}
		// SBC dp, X
	case 0xF5: {

		int16_t value = GetValue(AddMode::DirectIndexedX, !(P & MFlag));

		SBC(value);

		return (!(P & MFlag) ? 5 : 4) + extraCycles;
	}
		// INC dp, X
	case 0xF6: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= DP << 8;
		add += X;

		int c = 0;
		if (DP & 0xFF) {
			c++;
		}

		uint16_t value = ReadMemory(add, true);

		if (!(P & MFlag)) {
			value |= ReadMemory(add + 1, true) << 8;
			c += 2;
			value++;
		}
		else {
			uint8_t l = value & 0xFF;
			l++;
			value &= 0xFF00;
			value |= l;
		}

		CheckNFlag(value, true, false);
		CheckZFlag(value, true, false);

		WriteMemory(add, (value & 0xFF), true);
		if (!(P & MFlag)) {
			WriteMemory(add + 1, value >> 8, true);
		}

		return 6 + c;
	}
		// SBC [dp], Y
	case 0xF7: {

		int16_t value = GetValue(AddMode::DirectIndirectIndexedLong, !(P & MFlag));

		SBC(value);

		return (!(P & MFlag) ? 7 : 6) + extraCycles;
	}
		// SED
	case 0xF8:

		P |= DFlag;

		return 2;

		// SBC addr, Y
	case 0xF9: {

		int16_t value = GetValue(AddMode::AbsoluteIndexedY, !(P & MFlag));

		SBC(value);

		return (!(P & MFlag) ? 5 : 4) + extraCycles;
	}
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

		emulationMode = !emulationMode;

		if (emulationMode) {
			P &= ~CFlag;
			P |= (MFlag | XFlag);
			X &= 0xFF;
			Y &= 0xFF;
			SP = 0x01FF;
		}
		else {
			P |= CFlag;
		}

		return 2;

		// JSR (addr, X)
	case 0xFC: {

		uint16_t indAdd = ReadMemory((PB << 16) | PC++, true);
		indAdd |= ReadMemory((PB << 16) | PC++, true) << 8;
		indAdd += X;

		uint16_t add = ReadMemory(indAdd, false);
		add |= ReadMemory(indAdd + 1, false) << 8;


		Push((PC & 0xFF00) >> 8);
		Push(PC & 0xFF);

		PC = add;

		return 8;
	}
		// SBC addr, X
	case 0xFD: {

		int16_t value = GetValue(AddMode::AbsoluteIndexedX, !(P & MFlag));

		SBC(value);

		return (!(P & MFlag) ? 5 : 4) + extraCycles;
	}
		// INC addr, X
	case 0xFE: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;
		add += X;

		uint16_t value = ReadMemory(add, false);

		int c = 0;
		if (!(P & MFlag)) {
			value |= ReadMemory(add + 1, false) << 8;
			c += 2;
			value++;
		}
		else {
			uint8_t l = value & 0xFF;
			l++;
			value &= 0xFF00;
			value |= l;
		}

		CheckNFlag(value, true, false);
		CheckZFlag(value, true, false);

		WriteMemory(add, (value & 0xFF), false);
		if (!(P & MFlag)) {
			WriteMemory(add + 1, value >> 8, false);
		}

		return 7 + c;
	}
		// SBC long, X
	case 0xFF: {

		int16_t value = GetValue(AddMode::AbsoluteIndexedLong, !(P & MFlag));

		SBC(value);

		return !(P & MFlag) ? 6 : 5;
	}

	}

	return 2;
}

uint16_t CPURicoh::GetValue(AddMode addMode, bool is16)
{
	uint16_t value = 0;

	extraCycles = 0;

	switch (addMode) {

	case AddMode::Immediate: {
		value = ReadMemory((PB << 16) | PC++, true);

		if (is16) {
			value |= ReadMemory((PB << 16) | PC++, true) << 8;
		}
		break;
	}

	case AddMode::Absolute: {
		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;

		value = ReadMemory(add, false);

		if (is16) {
			value |= ReadMemory(add + 1, false) << 8;
		}

		break;
	}

	case AddMode::AbsoluteLong: {

		uint32_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;
		add |= ReadMemory((PB << 16) | PC++, true) << 16;

		value = ReadMemory(add, true);

		if (is16) {
			value |= ReadMemory(add + 1, true) << 8;
		}

		break;
	}

	case AddMode::AbsoluteIndexedX: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;

		if (!(P & XFlag) || ((add & 0xFF00) != ((add + X) & 0xFF00))) {
			extraCycles++;
		}
		add += X;

		value = ReadMemory(add, true);

		if (is16) {
			value |= ReadMemory(add + 1, true) << 8;
		}

		break;
	}

	case AddMode::AbsoluteIndexedY: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;

		if (!(P & XFlag) || ((add & 0xFF00) != ((add + X) & 0xFF00))) {
			extraCycles++;
		}
		add += Y;

		value = ReadMemory(add, true);

		if (is16) {
			value |= ReadMemory(add + 1, true) << 8;
		}

		break;
	}

	case AddMode::AbsoluteIndexedLong: {

		uint32_t add = ReadMemory((PB << 16) | PC++, true);
		add |= ReadMemory((PB << 16) | PC++, true) << 8;
		add |= ReadMemory((PB << 16) | PC++, true) << 16;

		add += X;

		value = ReadMemory(add, true);

		if (is16) {
			value |= ReadMemory(add + 1, true) << 8;
		}

		break;
	}

	case AddMode::Direct: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add |= DP << 8;

		if (DP & 0xFF) {
			extraCycles++;
		}

		value = ReadMemory(add, true);

		if (is16) {
			value |= ReadMemory(add + 1, true) << 8;
		}

		break;
	}

	case AddMode::DirectIndirect: {

		uint16_t indAdd = ReadMemory((PB << 16) | PC++, true);
		indAdd |= DP << 8;

		if (DP & 0xFF) {
			extraCycles++;
		}

		uint16_t add = ReadMemory(indAdd, true);
		add |= ReadMemory(indAdd + 1, true) << 8;

		value = ReadMemory(add, true);

		if (is16) {
			value |= ReadMemory(add + 1, true) << 8;
		}

		break;
	}

	case AddMode::DirectIndirectLong: {

		uint16_t indAdd = ReadMemory((PB << 16) | PC++, true);
		indAdd |= DP << 8;

		if (DP & 0xFF) {
			extraCycles++;
		}

		uint32_t add = ReadMemory(indAdd, true);
		add |= ReadMemory(indAdd + 1, true) << 8;
		add |= ReadMemory(indAdd + 2, true) << 16;

		value = ReadMemory(add, true);

		if (is16) {
			value |= ReadMemory(add + 1, true) << 8;
		}

		break;
	}

	case AddMode::DirectIndirectIndexedLong: {

		uint16_t indAdd = ReadMemory((PB << 16) | PC++, true);
		indAdd |= DP << 8;

		if (DP & 0xFF) {
			extraCycles++;
		}

		uint32_t add = ReadMemory(indAdd, true);
		add |= ReadMemory(indAdd + 1, true) << 8;
		add |= ReadMemory(indAdd + 2, true) << 16;
		add += Y;

		value = ReadMemory(add, true);

		if (is16) {
			value |= ReadMemory(add + 1, true) << 8;
		}

		break;
	}

	case AddMode::DirectIndirectIndexed: {

		uint16_t indAdd = ReadMemory((PB << 16) | PC++, true);
		indAdd |= DP << 8;

		if (DP & 0xFF) {
			extraCycles++;
		}

		uint16_t add = ReadMemory(indAdd, true);
		add |= ReadMemory(indAdd + 1, true) << 8;

		if (!(P & XFlag) || ((add & 0xFF00) != ((add + Y) & 0xFF00))) {
			extraCycles++;
		}
		add += Y;

		value = ReadMemory(add, true);

		if (is16) {
			value |= ReadMemory(add + 1, true) << 8;
		}

		break;
	}

	case AddMode::DirectIndexedIndirect: {

		uint16_t indAdd = ReadMemory((PB << 16) | PC++, true);
		indAdd |= DP << 8;
		indAdd += X;

		if (DP & 0xFF) {
			extraCycles++;
		}

		uint16_t add = ReadMemory(indAdd, true);
		add |= ReadMemory(indAdd + 1, true) << 8;

		value = ReadMemory(add, true);

		if (is16) {
			value |= ReadMemory(add + 1, true) << 8;
		}
		
		break;
	}

	case AddMode::DirectIndexedX: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);

		add |= DP << 8;
		add += X;

		value = ReadMemory(add, true);

		if (DP & 0xFF) {
			extraCycles++;
		}

		if (is16) {
			value |= ReadMemory(add + 1, true) << 8;
		}

		break;
	}

	case AddMode::DirectIndexedY: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);

		add |= DP << 8;
		add += Y;

		value = ReadMemory(add, true);

		if (DP & 0xFF) {
			extraCycles++;
		}

		if (is16) {
			value |= ReadMemory(add + 1, true) << 8;
		}

		break;
	}

	case AddMode::StackRelative: {

		uint16_t add = ReadMemory((PB << 16) | PC++, true);
		add += SP;

		value = ReadMemory(add, true);

		if (is16) {
			value |= ReadMemory(add + 1, true) << 8;
		}

		break;
	}

	case AddMode::StackRelativeIndirectIndexed: {

		uint16_t indAdd = ReadMemory((PB << 16) | PC++, true);
		indAdd += SP;

		uint16_t add = ReadMemory(indAdd, true);
		add |= ReadMemory(indAdd + 1, true) << 8;
		add += Y;

		value = ReadMemory(add, true);

		if (is16) {
			value |= ReadMemory(add + 1, true) << 8;
		}

		break;
	}

	}

	return value;
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

void CPURicoh::ADC(uint16_t value) {

	uint16_t prevA;
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
}

void CPURicoh::AND(uint16_t value) {

	if (!(P & MFlag)) {

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
}

void CPURicoh::CMP(uint16_t value) {

	if (!(P & MFlag)) {

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
}

void CPURicoh::CPX(uint16_t value) {

	if (!(P & XFlag)) {

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

	}
	else {
		if (value == (X & 0xFF)) {
			P |= ZFlag;
		}
		else {
			P &= ~ZFlag;
		}

		if ((X & 0xFF) < value) {
			P &= ~CFlag;
		}
		else {
			P |= CFlag;
		}

		value = (X & 0xFF) - value;
	}

	CheckNFlag(value, false, true);
}

void CPURicoh::CPY(uint16_t value) {

	if (!(P & XFlag)) {

		if (value == Y) {
			P |= ZFlag;
		}
		else {
			P &= ~ZFlag;
		}

		if (Y < value) {
			P &= ~CFlag;
		}
		else {
			P |= CFlag;
		}

		value = Y - value;

	}
	else {
		if (value == (Y & 0xFF)) {
			P |= ZFlag;
		}
		else {
			P &= ~ZFlag;
		}

		if ((Y & 0xFF) < value) {
			P &= ~CFlag;
		}
		else {
			P |= CFlag;
		}

		value = (Y & 0xFF) - value;
	}

	CheckNFlag(value, false, true);
}

void CPURicoh::EOR(uint16_t value) {
	if (!(P & MFlag)) {

		A ^= value;

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
		uint8_t l = A & 0xFF;
		A &= 0xFF00;
		A |= (value ^ l);

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
}

void CPURicoh::ORA(uint16_t value) {

	if (!(P & MFlag)) {

		A |= value;

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
		A |= (value & 0x00FF);

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
}

void CPURicoh::SBC(uint16_t value) {

	uint16_t prevA;
	int8_t C = (P & CFlag) ? 1 : 0;

	value = ~value;

	if (P & MFlag) {
		uint16_t result;
		// Decimal Mode
		if (P & DFlag) {
			result = (A & 0x0F) + (uint8_t)(value & 0x0F) + C;
			if (result <= 0x0F) {
				result -= 0x06;
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

			if (result <= 0xFF) {
				result -= 0x60;
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

		if (result > 0xFF) {
			P |= CFlag;
		}
		else {
			P &= ~CFlag;
		}
	}
	else {
		uint32_t result;
		if (P & DFlag) {

			result = (A & 0x0F) + (value & 0x0F) + C;
			if (result <= 0x0F) {
				result -= 0x06;
			}
			C = (result > 0x000F);
			result = (A & 0xF0) + (value & 0xF0) + (C << 4) + (result & 0x0F);

			if (result <= 0x00FF) {
				result -= 0x60;
			}
			C = (result > 0x00FF);
			result = (A & 0xF00) + (value & 0xF00) + (C << 8) + (result & 0xFF);

			if (result <= 0x0FFF) {
				result -= 0x600;
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

			if (result <= 0xFFFF) {
				result -= 0x6000;
			}

			A = result;
		}
		else {
			prevA = (uint16_t)A;

			result = (uint16_t)A + (uint16_t)value + C;

			if (~(prevA ^ value) & (prevA ^ result) & 0x8000) {
				P |= VFlag;
			}
			else {
				P &= ~VFlag;
			}

			A = result;
		}

		if (result > 0xFFFF) {
			P |= CFlag;
		}
		else {
			P &= ~CFlag;
		}
	}

	CheckNFlag(A, true, false);

	CheckZFlag(A, true, false);
}