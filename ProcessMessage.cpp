#include "ProcessMessage.hpp"

#include <Windows.h>


ProcessMessage::ProcessMessage() //: originalNumber_{ 0 }, dataValue_{ 0 }, subVectorSize_{ 0 }
{
	ZeroMemory(textOutput_, sizeof(textOutput_));
}

void ProcessMessage::setText(const std::string& text)
{
	size_t len = (text.length() >= MAX_TEXT_SIZE) ? MAX_TEXT_SIZE - 1 : text.length();
	std::memcpy(textOutput_, text.c_str(), len);
	textOutput_[len] = '\0'; // Ensure null-termination
}

// Sends itself across the provided Windows pipe handle
bool ProcessMessage::send(HANDLE hPipe)
{
	DWORD bytesWritten;
	BOOL success = WriteFile(
		hPipe,
		this,                 // Sends the object binary payload directly
		sizeof(ProcessMessage),
		&bytesWritten,
		nullptr
	);
	return success && (bytesWritten == sizeof(ProcessMessage));
}

// Populates itself by reading from the provided Windows pipe handle
bool ProcessMessage::receive(HANDLE hPipe)
{
	DWORD bytesRead;
	BOOL success = ReadFile(
		hPipe,
		this,                 // Overwrites current instance memory with incoming packet data
		sizeof(ProcessMessage),
		&bytesRead,
		nullptr
	);
	return success && (bytesRead > 0);
}

char const* ProcessMessage::getTestOutput() const
{
	return textOutput_;
}


