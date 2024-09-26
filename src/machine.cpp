// Written by Wojciech Kieloch circa 2022.

#include "machine.h"

#include <fstream>
#include <thread>
#include <filesystem>

Machine::Machine(uint32_t displayScaleArg)
{
	srand((unsigned int)time(nullptr));
	scale = displayScaleArg;
	DisplayHeight = displayScaleArg * DISPLAY_ARRAY_HEIGHT;
	DisplayWidth = displayScaleArg * DISPLAY_ARRAY_WIDTH;
}

Machine::~Machine()
{
	EndDisplay();
}

void Machine::LoadFonts()
{
	for (int i = 0; i < FONTS_ARRAY_SIZE; i++)
		Memory[i] = Fonts[i];
}

void Machine::LaunchRom(std::string filePath)
{
	ResetMachine();
	InitializeDispaly();
	LoadRom(filePath);

	int insCount = -1;

	while (!quitFlag)
	{
		if (ProgramCounter >= MEMORY_SIZE)
			throw std::runtime_error("ProgamCounter out of bounds.");

		HandleInput();

		EmulateIns();

		insCount = (insCount + 1) % insPerTimer;

		std::this_thread::sleep_for(std::chrono::microseconds(insDeltaMicroS));

		if (insCount == 0)
		{
			DelayTimer--;
			if (DelayTimer < 0)
				DelayTimer = 0;
		}
	}

	EndDisplay();
}

void Machine::HandleInput()
{
	while (SDL_PollEvent(&Event))
	{
		if (Event.type == SDL_WINDOWEVENT)
		{
			if (Event.window.event == SDL_WINDOWEVENT_CLOSE)
			{
				quitFlag = true;
				break;
			}
		}
		else if (Event.type == SDL_KEYDOWN)
		{
			std::string pressedCharacter = SDL_GetKeyName(Event.key.keysym.sym);
			if (pressedCharacter.length() == 1 && Keyboard.count(pressedCharacter[0]) == 1)
			{
				char hexForm = fromHex(Keyboard[pressedCharacter[0]]);
				Keys[hexForm] = true;
			}
		}
		else if (Event.type == SDL_KEYUP)
		{
			std::string liftedCharacter = SDL_GetKeyName(Event.key.keysym.sym);
			if (liftedCharacter.length() == 1 && Keyboard.count(liftedCharacter[0]) == 1)
			{
				char hexForm = fromHex(Keyboard[liftedCharacter[0]]);
				Keys[hexForm] = false;
			}
		}
	}
}

void Machine::NoSuchOpcode(uint16_t opcode)
{
	throw std::invalid_argument("No such opcode exists!: " + std::to_string(opcode));
}

void Machine::EmulateIns()
{
	currentOpcode = MergeBytes(Memory[ProgramCounter], Memory[ProgramCounter + 1]);
	HandleOpcode(currentOpcode);
}

void Machine::LoadRom(std::string filePath)
{
	std::ifstream file;
	file.open(filePath, std::ios::binary);
	if (file.good())
	{
		uintmax_t fileSize = getFileSize(filePath);
		const int startAddress = 0x200;
		const int memoryLeft = MEMORY_SIZE - 0x200;
		if (memoryLeft < fileSize)
			throw std::runtime_error("The file is too big.");

		file.read(reinterpret_cast<char *>(Memory + startAddress), fileSize);
	}
	else
		throw std::runtime_error("File error!");
}

uintmax_t Machine::getFileSize(const std::string &filePath)
{
	const std::filesystem::path fsPath(filePath);
	return std::filesystem::file_size(fsPath);
}

uint16_t Machine::GetValueFromBits(uint16_t shortType, unsigned int startPosition, unsigned int lenght)
{
	const unsigned int numberOfBytes = sizeof(uint16_t) * 8;
	const unsigned int mask = ((1 << lenght) - 1);
	const unsigned int isolated = shortType >> (numberOfBytes - (startPosition + lenght));
	return isolated & mask;
}

void Machine::InitializeDispaly()
{
	if (displayInitFlag)
		EndDisplay();

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		throw std::runtime_error("SDL could not initialize! SDL_Error: " + std::string(SDL_GetError()));
	else if (SDL_CreateWindowAndRenderer(DisplayWidth, DisplayHeight, 0, &AppWindow, &Renderer) != 0)
		throw std::runtime_error("SDL could not initialize a window and a renderer! SDL_Error: " + std::string(SDL_GetError()));
	displayInitFlag = true;
}

void Machine::UpdateDisplay()
{
	ClearDisplay();
	SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 0); // White

	for (int i = 0; i < DISPLAY_ARRAY_HEIGHT; i++)
	{
		for (int j = 0; j < DISPLAY_ARRAY_WIDTH; j++)
		{
			if (bDisplay[i][j] == true)
			{
				SDL_Rect rect;
				rect.y = i * scale;
				rect.x = j * scale;
				rect.h = scale;
				rect.w = scale;
				SDL_RenderFillRect(Renderer, &rect);
			}
		}
	}

	SDL_RenderPresent(Renderer);
}

void Machine::ClearDisplay()
{
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0); // Black
	SDL_RenderClear(Renderer);
}

void Machine::EndDisplay()
{
	if (displayInitFlag)
	{
		SDL_DestroyRenderer(Renderer);
		Renderer = nullptr;
		SDL_DestroyWindow(AppWindow);
		AppWindow = nullptr;
		SDL_Quit();
	}
}

uint8_t Machine::fromHex(char symbol)
{
	if (symbol >= 'A')
		return (10 + (symbol - 'A'));
	else
		return (symbol - '0');
}

uint16_t Machine::MergeBytes(uint8_t FirstByte, uint8_t SecondByte)
{
	const uint16_t opcode = FirstByte << 8 | SecondByte;
	return opcode;
}

void Machine::ClearDisplayMatrix()
{
	for (uint i = 0; i < DISPLAY_ARRAY_HEIGHT; i++)
		for (uint j = 0; j < DISPLAY_ARRAY_WIDTH; j++)
			bDisplay[i][j] = false;
}

void Machine::ResetMachine()
{
	ProgramCounter = 0x200;
	IndexRegister = 0;
	StackPointer = 0;
	ClearDisplayMatrix();
	EndDisplay();

	for (uint i = 0; i < STACK_SIZE; i++)
		Stack[i] = 0;

	for (uint i = 0; i < REGISTERS_COUNT; i++)
		Registers[i] = 0;

	for (uint i = FONTS_ARRAY_SIZE; i < MEMORY_SIZE; i++)
		Memory[i] = 0;

	for (uint i = 0; i < 16; i++)
		Keys[i] = false;

	LoadFonts();
	DelayTimer = 0;
	quitFlag = false;
	displayInitFlag = false;
}

void Machine::BeepFor(uint16_t val)
{
	const double beepingTimeMicroSec = val * timersDeltaMicroS;
	// not implemented.
}