// Written by Wojciech Kieloch circa 2022.

#include "machine.h"

void Machine::CLS() // 00E0
{
	ClearDisplayMatrix(); // fix that.
	UpdateDisplay();
	ProgramCounter += 2;
}

void Machine::RET() // 00EE
{
	ProgramCounter = Stack[StackPointer];
	StackPointer--;
	ProgramCounter += 2;
}

void Machine::JMP(uint address) // 1NNN
{
	ProgramCounter = address;
}

void Machine::CALL_NNN(uint address) // 2NNN
{
	StackPointer++;
	Stack[StackPointer] = ProgramCounter;
	ProgramCounter = address;
}

void Machine::SE_XNN(uint X, uint value) // 3XNN
{
	if (Registers[X] == value)
		ProgramCounter += 4;
	else ProgramCounter += 2;
}

void Machine::SNE_XNN(uint X, uint value) // 4XNN
{
	if (Registers[X] != value)
		ProgramCounter += 4;
	else ProgramCounter += 2;
}

void Machine::SE_XY(uint X, uint Y) // 5XY0
{
	if (Registers[X] == Registers[Y])
		ProgramCounter += 4;
	else ProgramCounter += 2;
}

void Machine::LD_XNN(uint X, uint value) // 6XNN
{
	Registers[X] = value;
	ProgramCounter += 2;
}

void Machine::ADD_XNN(uint X, uint value) // 7XNN
{
	Registers[X] += value;
	ProgramCounter += 2;
}

void Machine::LD_XY(uint X, uint Y) // 8XY0
{
	Registers[X] = Registers[Y];
	ProgramCounter += 2;
}

void Machine::OR_XY(uint X, uint Y) // 8XY1
{
	Registers[X] |= Registers[Y];
	ProgramCounter += 2;
}

void Machine::AND_XY(uint X, uint Y) // 8XY2
{
	Registers[X] &= Registers[Y];
	ProgramCounter += 2;
}

void Machine::XOR_XY(uint X, uint Y) // 8XY3
{
	Registers[X] ^= Registers[Y];
	ProgramCounter += 2;
}

void Machine::ADD_XY(uint X, uint Y) // 8XY4
{
	if (Registers[X] + Registers[Y] > 0xFF)
		Registers[VF] = 1;
	else Registers[VF] = 0;
	Registers[X] += Registers[Y];
	ProgramCounter += 2;
}

void Machine::SUB_XY(uint X, uint Y) // 8XY5
{
	if (Registers[X] >= Registers[Y]) // Changed.
		Registers[VF] = 1;
	else Registers[VF] = 0;
	Registers[X] -= Registers[Y];
	ProgramCounter += 2;
}

void Machine::SHR_XY(uint X, uint Y) // 8XY6
{
	Registers[VF] = Registers[X] & 0x01;
	Registers[X] /= 2;
	ProgramCounter += 2;
}

void Machine::SUBN_XY(uint X, uint Y) // 8XY7
{
	if (Registers[Y] >= Registers[X]) // Changed.
		Registers[VF] = 1;
	else Registers[VF] = 0;
	Registers[X] = Registers[Y] - Registers[X];
	ProgramCounter += 2;
}

void Machine::SHL_XY(uint X, uint Y) // 8XYE
{
	Registers[VF] = (Registers[X] & 0x80) >> 7;
	Registers[X] *= 2;
	ProgramCounter += 2;
}

void Machine::SNE_XY(uint X, uint Y) // 9XY0
{
	if (Registers[X] != Registers[Y])
		ProgramCounter += 4;
	else ProgramCounter += 2;
}

void Machine::LD_INNN(uint address) // ANNN
{
	IndexRegister = address;
	ProgramCounter += 2;
}

void Machine::JMP_0NNN(uint address) // BNNN
{
	ProgramCounter = Registers[0] + address;
}

void Machine::RND_XNN(uint X, uint value) // CXNN
{
	Registers[X] = (rand() % 256) & value;
	ProgramCounter += 2;
}

void Machine::DRW_XYN(uint X, uint Y, uint value) // DXYN
{
	uint ycoord = Registers[Y] % DISPLAY_ARRAY_HEIGHT;
	bool pixelFlipped = false;
	for (uint row = 0; row < value; row++)
	{
		uint8_t byte = Memory[IndexRegister + row];
		for (uint col = 0; col < 8; col++)
		{
			uint xcoord = (Registers[X] + col) % DISPLAY_ARRAY_WIDTH;
			bool currentPosition = bDisplay[ycoord][xcoord];
			uint oneBit = byte & (0x01 << (7 - col));
			if (oneBit > 0)
			{
				if (currentPosition == 1)
				{
					bDisplay[ycoord][xcoord] = 0;
					pixelFlipped = true;
				}
				else
				{
					bDisplay[ycoord][xcoord] = 1;
				}		
			}
		}
		ycoord++;
		ycoord %= DISPLAY_ARRAY_HEIGHT;
	}
	Registers[VF] = (pixelFlipped ? 1 : 0);
	UpdateDisplay();

	ProgramCounter += 2;
}

void Machine::SKP_X(uint X) // EX9E
{
	if (Keys[Registers[X]] == true) // changed.
		ProgramCounter += 4;
	else ProgramCounter += 2;
}

void Machine::SKNP_X(uint X) // EXA1
{
	if (Keys[Registers[X]] != true)
		ProgramCounter += 4;
	else ProgramCounter += 2;
}

void Machine::LD_XDT(uint X) // FX07
{
	Registers[X] = (uint8_t)DelayTimer;
	ProgramCounter += 2;
}

void Machine::LD_XK(uint X) // FX0A
{
	// to mozna zmienic.
	bool keyPressed = false;
	do
	{
		while (SDL_PollEvent(&Event) != 0)
		{
			if (Event.type == SDL_WINDOWEVENT)
			{
				if (Event.window.event == SDL_WINDOWEVENT_CLOSE)
				{
					quitFlag = true;
					keyPressed = true;
					break;
				}
			}
			else if (Event.type == SDL_KEYDOWN)
			{
				std::string pressedCharacter = SDL_GetKeyName(Event.key.keysym.sym);
				if (pressedCharacter.length() == 1 && Keyboard.count(pressedCharacter[0]) == 1)
				{
					Registers[X] = fromHex(Keyboard[pressedCharacter[0]]);
					keyPressed = true;
					break;
				}
			}
		}
	} while (!keyPressed);
	ProgramCounter += 2;
}

void Machine::LD_DTX(uint X) // FX15
{
	DelayTimer = Registers[X];
	ProgramCounter += 2;
}

void Machine::LD_STX(uint X) // FX18
{
	BeepFor(Registers[X]);
	ProgramCounter += 2;
}

void Machine::ADD_IX(uint X) // FX1E
{
	// changed.
	if (IndexRegister + Registers[X] > 0xFFF)
		Registers[VF] = 1;
	else Registers[VF] = 0;
	IndexRegister += Registers[X];
	ProgramCounter += 2;
}

void Machine::LD_FX(uint X) // FX29
{
	IndexRegister = Registers[X] * 0x05;
	ProgramCounter += 2;
}

void Machine::LD_BX(uint X) // FX33
{
	Memory[IndexRegister] = Registers[X] / 100;
	Memory[IndexRegister + 1] = (Registers[X] / 10) % 10;
	Memory[IndexRegister + 2] = Registers[X] % 10;
	ProgramCounter += 2;
}

void Machine::LD_IX(uint X) // FX55
{
	for (uint i = 0; i <= X; i++)
		Memory[IndexRegister + i] = Registers[i];

	// On the original interpreter?
	IndexRegister += X + 1;
	ProgramCounter += 2;
}

void Machine::LD_XI(uint X) // FX65
{
	for (uint i = 0; i <= X; i++)
		Registers[i] = Memory[IndexRegister + i];

	// On the original interpreter?
	IndexRegister += X + 1;
	ProgramCounter += 2;
}