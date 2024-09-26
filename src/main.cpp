// Written by Wojciech Kieloch circa 2022.

#include <iostream>
#include <filesystem>
#include <map>
#include <thread>

#include "machine.h"

using namespace std;

void printInterface(const std::string &pathToDir, map<string, string> &numberToPath);

int main(int argc, char *argv[])
{
	string selection = "";
	const string pathToDir = "../roms/";
	map<string, string> numberToPath;
	const uint displayScale = 10;
	unique_ptr<Machine> Chip8(new Machine(displayScale));

	while (true)
	{
		printInterface(pathToDir, numberToPath);

		cin >> selection;
		if (selection == "q")
			break;
		else if (numberToPath.count(selection))
			Chip8->LaunchRom(numberToPath[selection]);
		else
			cout << "Invalid choice" << endl;

		numberToPath.clear();

		const uint waitDeltaMS = 10;
		std::this_thread::sleep_for(std::chrono::milliseconds(waitDeltaMS)); // let the program sleep.
	}

	return 0;
}

void printInterface(const std::string &pathToDir, map<string, string> &numberToPath)
{
	cout << "================================\n";
	cout << "CHIP8 INTERPRETER (select a number, q to exit)\n";
	int i = 1;
	for (const filesystem::directory_entry &entry : filesystem::directory_iterator(pathToDir))
	{
		std::cout << i << ' ' << entry.path() << "\n";
		numberToPath[to_string(i)] = entry.path().string();
		i++;
	}
	cout << endl;
}
