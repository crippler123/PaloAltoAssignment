#ifndef PROCESS_MESSAGE_HPP
#define PROCESS_MESSAGE_HPP

#include <Windows.h>

#include <string>

class ProcessMessage {

private:

	static constexpr int MAX_TEXT_SIZE = 4096;

	char textOutput_[MAX_TEXT_SIZE];

public:

	ProcessMessage();

	//Prepeare buffer to send
	void setText(const std::string& text);
	// Sends itself across the provided Windows pipe handle
	bool send(HANDLE hPipe);
	// Populates itself by reading from the provided Windows pipe handle
	bool receive(HANDLE hPipe);
	char const* getTestOutput() const;


	//Rule of 5
	~ProcessMessage() = default;
	ProcessMessage(ProcessMessage const&) = delete;
	ProcessMessage& operator=(ProcessMessage const&) = delete;
	ProcessMessage(ProcessMessage&&) = delete;
	ProcessMessage& operator=(ProcessMessage&&) = delete;
};


#endif//PROCESS_MESSAGE_HPP