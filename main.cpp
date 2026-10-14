
#include <iostream>
#include <vector>
#include <random>
#include <windows.h>
#include <string>

#include "AuxilaryFunctions.hpp"

const char* PIPE_NAME = "\\\\.\\pipe\\MyRandomNumberPipe";
constexpr int minVal = 1;
constexpr int maxVal = 999;

int main(int argc, char* argv[])
{

	if (argc < 2)
	{
		std::cerr << "Usage Error. Run: " << argv[0] << " -i\n";

		return 1;
	}

	std::cout << argv[1] << std::endl;

	if (strcmp(argv[1], "-i") == 0)
	{
		runInputProcess(PIPE_NAME, minVal, maxVal);
	}
	else if (strcmp(argv[1], "-o") == 0)
	{
		runOutputProcess(PIPE_NAME);
	}
	else
	{
		std::cerr << "Invalid Argument. Use '-i' or '-o'\n";
		return 1;
	}

	return 0;
}

