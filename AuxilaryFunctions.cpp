#include "AuxilaryFunctions.hpp"

#include <random>
#include <iostream>

#include "ProcessMessage.hpp"


std::vector<int> generateRandomVector(int size, int min_val, int max_val)
{
	if (min_val > max_val) {
		std::swap(min_val, max_val); // הפיכת הערכים כדי למנוע קריסה
	}

	std::vector<int> vec(size);

	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_int_distribution<> distr(min_val, max_val);

	for (int i = 0; i < size; ++i) {
		vec[i] = distr(gen);
	}

	return vec;
}

bool startsWithOne(int num)
{
	if (num <= 0) return false;
	while (num >= 10) {
		num /= 10;
	}
	return num == 1;
}

int getDigitsMultiplication(int num)
{
	int product = 1;
	while (num > 0) {
		product *= (num % 10);
		num /= 10;
	}
	return product;
}

void runInputProcess(char const* pipeName, int minVal, int maxVal)
{
	HANDLE hPipe = CreateNamedPipeA(
		pipeName, PIPE_ACCESS_OUTBOUND,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		1, sizeof(ProcessMessage), sizeof(ProcessMessage), 0, nullptr
	);

	if (hPipe == INVALID_HANDLE_VALUE) {
		std::cerr << "[P1] Failed to create Named Pipe. Error: " << GetLastError() << "\n";
		return;
	}

	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	char exePath[MAX_PATH];
	GetModuleFileNameA(nullptr, exePath, MAX_PATH);
	std::string commandLine = std::string(exePath) + " -o";

	if (!CreateProcessA(nullptr, (LPSTR)commandLine.c_str(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
		std::cerr << "[P1] CreateProcess failed. Error: " << GetLastError() << "\n";
		CloseHandle(hPipe);
		return;
	}

	std::cout << "[P1] Waiting for Process 2 to connect to the pipe...\n";
	BOOL connected = ConnectNamedPipe(hPipe, nullptr) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
	if (!connected) {
		CloseHandle(hPipe);
		return;
	}

	int count;
	std::cout << "[P1] Enter how many random numbers to generate: ";
	std::cin >> count;
	if (count <= 0)
	{
		CloseHandle(hPipe);
		return;
	}

	std::vector<int> main_vector = generateRandomVector(count, minVal, maxVal);

	// Loop builds complete presentation strings entirely on P1's end
	for (int num : main_vector) {
		std::string output_block = "";

		if (num % 2 != 0)
		{
			output_block += "Category: Odd numbers\n";
			output_block += std::to_string(num) + ": this is an odd number\n";
		}
		else if (startsWithOne(num))
		{
			output_block += "Category: Numbers that start with a digit equal to 1\n";
			output_block += std::to_string(num) + ": starts with 1 [origin list: ";

			std::vector<int> sub_vector = generateRandomVector(num, 1, 999);
			for (int x : sub_vector) output_block += std::to_string(x) + " ";
			output_block += "]\n";

			// Sorting logic happens inside P1
			std::sort(sub_vector.begin(), sub_vector.end());
			output_block += "Ordered list: ";
			for (int x : sub_vector) output_block += std::to_string(x) + " ";
			output_block += "\n";
		}
		else if (num >= 100 && num <= 999)
		{
			int mult = getDigitsMultiplication(num);

			output_block += "Category: Numbers containing 3 digits\n";
			output_block += std::to_string(num) + ": the digit's multiplications are equal to " + std::to_string(mult) + "\n";
		}
		else
		{
			output_block += "Category: The rest of the numbers\n";
			output_block += std::to_string(num) + ": this number belongs to the others\n";
		}

		output_block += "-------------------------------------------\n";

		// Pack the final built string into the message class and send
		ProcessMessage msg;
		msg.setText(output_block);
		msg.send(hPipe);
	}

	FlushFileBuffers(hPipe);
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);

	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

void runOutputProcess(char const* pipeName)
{
	Sleep(500);

	HANDLE hPipe = CreateFileA(pipeName, GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr);

	if (hPipe == INVALID_HANDLE_VALUE)
	{
		std::cerr << "[P2] Failed to connect to pipe.\n";
		return;
	}

	ProcessMessage msg;

	std::cout << "\n--- [P2] Stream Console Output ---\n\n";

	// P2 has absolutely zero knowledge of categories. It just receives text and prints it.
	while (msg.receive(hPipe))
	{
		std::cout << msg.getTestOutput();
	}

	CloseHandle(hPipe);
}