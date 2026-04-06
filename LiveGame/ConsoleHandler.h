#pragma once

#include <iostream>
#include <windows.h>
#include <sstream>
#include "Field.h"
#include "KeyListener.h"

enum class ConsoleStates { exiting, working, paused, changing_tick, changing_delay, show_help, dumping };

class ConsoleHandler
{
private:
	HANDLE hOutput;
	std::vector<HANDLE> double_buffer;

	void write_to_void(void* dst, void* src, size_t size);
public:
	char stream[256];
	ConsoleStates state;

	ConsoleHandler();

	~ConsoleHandler();
	// check command from user

	void draw();

	void exec();

	std::string what();

	void* get_shared_data();

};