// Written by Wojciech Kieloch circa 2022.

#include "machine.h"

/*
/ This way of handling opcodes is naive. At least there is not too much of them.
*/

void Machine::HandleOpcode(uint16_t opcode)
{
	const int group = GetValueFromBits(opcode, 0, 4);
	switch (group)
	{

	case 0:
	{
		const int rest = GetValueFromBits(opcode, 4, 12);
		if (rest == 0x0E0)
			CLS();
		else if (rest == 0x0EE)
			RET();
		else NoSuchOpcode(opcode);
	}
	break;

	case 1:
	{
		const int address = GetValueFromBits(opcode, 4, 12);
		JMP(address);
	}
	break;

	case 2:
	{
		const int address = GetValueFromBits(opcode, 4, 12);
		CALL_NNN(address);
	}
	break;

	case 3:
	{
		const int X = GetValueFromBits(opcode, 4, 4);
		const int value = GetValueFromBits(opcode, 8, 8);
		SE_XNN(X, value);
	}
	break;

	case 4:
	{
		const int X = GetValueFromBits(opcode, 4, 4);
		const int value = GetValueFromBits(opcode, 8, 8);
		SNE_XNN(X, value);
	}
	break;

	case 5:
	{
		const int X = GetValueFromBits(opcode, 4, 4);
		const int Y = GetValueFromBits(opcode, 8, 4);
		SE_XY(X, Y);
	}
	break;

	case 6:
	{
		const int X = GetValueFromBits(opcode, 4, 4);
		const int value = GetValueFromBits(opcode, 8, 8);
		LD_XNN(X, value);
	}
	break;

	case 7:
	{
		const int X = GetValueFromBits(opcode, 4, 4);
		const int value = GetValueFromBits(opcode, 8, 8);
		ADD_XNN(X, value);
	}
	break;

	case 8:
	{	
		const int X = GetValueFromBits(opcode, 4, 4);
		const int Y = GetValueFromBits(opcode, 8, 4);
		const int instructionType = GetValueFromBits(opcode, 12, 4);

		switch (instructionType)
		{
		case 0:
			LD_XY(X, Y);
			break;
		case 1:
			OR_XY(X, Y);
			break;
		case 2:
			AND_XY(X, Y);
			break;
		case 3:
			XOR_XY(X, Y);
			break;
		case 4:
			ADD_XY(X, Y);
			break;
		case 5:
			SUB_XY(X, Y);
			break;
		case 6:
			SHR_XY(X, Y);
			break;
		case 7:
			SUBN_XY(X, Y);
			break;
		case 0xE:
			SHL_XY(X, Y);
			break;
		default:
			NoSuchOpcode(opcode);
			break;
		}
	}
	break;

	case 9:
	{
		const int X = GetValueFromBits(opcode, 4, 4);
		const int Y = GetValueFromBits(opcode, 8, 4);
		SNE_XY(X, Y);
	}
	break;

	case 0xA:
	{
		const int address = GetValueFromBits(opcode, 4, 12);
		LD_INNN(address);
	}
	break;

	case 0xB:
	{
		const int address = GetValueFromBits(opcode, 4, 12);
		JMP_0NNN(address);
	}
	break;

	case 0xC:
	{
		const int X = GetValueFromBits(opcode, 4, 4);
		const int value = GetValueFromBits(opcode, 8, 8);
		RND_XNN(X, value);
	}
	break;

	case 0xD:
	{
		const int X = GetValueFromBits(opcode, 4, 4);
		const int Y = GetValueFromBits(opcode, 8, 4);
		const int value = GetValueFromBits(opcode, 12, 4);
		DRW_XYN(X, Y, value);
	}
	break;

	case 0xE:
	{	
		const int X = GetValueFromBits(opcode, 4, 4);
		const int instructionType = GetValueFromBits(opcode, 8, 8);
		if (instructionType == 0x9E)
			SKP_X(X);
		else if (instructionType == 0xA1)
			SKNP_X(X);
		else NoSuchOpcode(opcode);
	}
	break;

	case 0xF:
	{	
		const int X = GetValueFromBits(opcode, 4, 4);
		const int instructionType = GetValueFromBits(opcode, 8, 8);
		switch (instructionType)
		{
		case 0x07:
			LD_XDT(X);
			break;
		case 0x0A:
			LD_XK(X);
			break;
		case 0x15:
			LD_DTX(X);
			break;
		case 0x18:
			LD_STX(X);
			break;
		case 0x1E:
			ADD_IX(X);
			break;
		case 0x29:
			LD_FX(X);
			break;
		case 0x33:
			LD_BX(X);
			break;
		case 0x55:
			LD_IX(X);
			break;
		case 0x65:
			LD_XI(X);
			break;
		default:
			NoSuchOpcode(opcode);
			break;
		}
	}
	break;

	default:
		
		break;
	}
}