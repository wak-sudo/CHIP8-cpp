// Written by Wojciech Kieloch circa 2022.

#pragma once

#include <unordered_map>
#include <SDL2/SDL.h>
#include <string>

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint;

#define DISPLAY_ARRAY_WIDTH 64
#define DISPLAY_ARRAY_HEIGHT 32
#define MEMORY_SIZE 4096
#define REGISTERS_COUNT 17
#define KEYBOARD_SIZE 16
#define STACK_SIZE 16
#define FONTS_ARRAY_SIZE 90

#define VF 0xF

class Machine
{
	// Machine:
	uint8_t Memory[MEMORY_SIZE];
	uint8_t Registers[REGISTERS_COUNT];

	int DelayTimer = 0;
	const uint timersDeltaMicroS = 16670;
	const uint insDeltaMicroS = 1667; // change this to change speed.
	const uint insPerTimer = timersDeltaMicroS / insDeltaMicroS;

	uint16_t IndexRegister = 0;
	uint16_t ProgramCounter = 0x200;
	uint16_t StackPointer = 0;
	uint16_t Stack[STACK_SIZE];

	uint16_t currentOpcode;

	// Display:
	bool displayInitFlag = false;
	uint scale = 1;
	uint DisplayHeight = DISPLAY_ARRAY_HEIGHT;
	uint DisplayWidth = DISPLAY_ARRAY_WIDTH;
	SDL_Window *AppWindow = nullptr;
	SDL_Renderer *Renderer = nullptr;
	bool bDisplay[DISPLAY_ARRAY_HEIGHT][DISPLAY_ARRAY_WIDTH];

	// Keyboard
	SDL_Event Event;
	bool Keys[KEYBOARD_SIZE];

	// Rest
	bool quitFlag = false;

	const uint8_t Fonts[FONTS_ARRAY_SIZE] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80, // F
	};

	std::unordered_map<char, char> Keyboard{
		{'1', '1'},
		{'2', '2'},
		{'3', '3'},
		{'4', 'C'},
		{'Q', '4'},
		{'W', '5'},
		{'E', '6'},
		{'R', 'D'},
		{'A', '7'},
		{'S', '8'},
		{'D', '9'},
		{'F', 'E'},
		{'Z', 'A'},
		{'X', '0'},
		{'C', 'B'},
		{'V', 'F'},
	};

	// Methods
	void LoadFonts();
	void InitializeDispaly();
	void UpdateDisplay();
	void EndDisplay();
	void ClearDisplay();

	void HandleOpcode(uint16_t opcode);
	void HandleInput();
	void BeepFor(uint16_t val);
	void EmulateIns();
	void ClearDisplayMatrix();

	void LoadRom(std::string filePath);
	void ResetMachine();

	static uint16_t GetValueFromBits(uint16_t, unsigned int, unsigned int);
	static uint16_t MergeBytes(uint8_t, uint8_t);
	static void NoSuchOpcode(uint16_t opcode);
	static uint8_t fromHex(char);
	static uint64_t getFileSize(const std::string &filePath);

	// Opcodes:
	void CLS();
	void RET();
	void JMP(uint);
	void CALL_NNN(uint);
	void SE_XNN(uint, uint);
	void SNE_XNN(uint, uint);
	void SE_XY(uint, uint);
	void LD_XNN(uint, uint);
	void ADD_XNN(uint, uint);
	void LD_XY(uint, uint);
	void OR_XY(uint, uint);
	void AND_XY(uint, uint);
	void XOR_XY(uint, uint);
	void ADD_XY(uint, uint);
	void SUB_XY(uint, uint);
	void SHR_XY(uint, uint);
	void SUBN_XY(uint, uint);
	void SHL_XY(uint, uint);
	void SNE_XY(uint, uint);
	void LD_INNN(uint);
	void JMP_0NNN(uint);
	void RND_XNN(uint, uint);
	void DRW_XYN(uint, uint, uint);
	void SKP_X(uint);
	void SKNP_X(uint);
	void LD_XDT(uint);
	void LD_XK(uint);
	void LD_DTX(uint);
	void LD_STX(uint);
	void ADD_IX(uint);
	void LD_FX(uint);
	void LD_BX(uint);
	void LD_IX(uint);
	void LD_XI(uint);

public:
	Machine(uint32_t displayScaleArg = 1);
	~Machine();
	void LaunchRom(std::string);
};
