#pragma once

#include <iostream>
#include <sstream>
#include "Field.h"
#include "KeyListener.h"

enum class ConsoleStates { exiting, working, paused, changing_tick, changing_delay, show_help, dumping };

class ConsoleHandler
{
private:
	// program write result of command
	void* shared_data;
	std::stringstream err_msg;
	static int32_t pos_at_line;
	static int32_t w8_key;

	void write_to_void(void* dst, void* src, size_t size);
public:
	char stream[256];
	ConsoleStates state;

	ConsoleHandler();

	~ConsoleHandler();
	// check command from user
	void ask();

	void exec();

	std::string what();

	void* get_shared_data();

};