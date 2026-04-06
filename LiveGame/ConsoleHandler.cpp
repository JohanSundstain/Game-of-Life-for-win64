#include "ConsoleHandler.h"
#include "Field.h"
#include <sstream>
#include "KeyListener.h"

static const char* help_info =
"\nSYNOPSIS online\n"
"\t<command> [params]\n"
"OPTIONS\n"
"\thelp\t\tPrint help information.\n"
"\ttick|t step\tSet number of steps for the simulation (default=1).\n"
"\tdump filename\tSave current map to the file with 'filename' (default=output.txt).\n"
"\tdelay|d sec\tSet time of delay in seconds for the simulation. (default=0.3f).\n"
"\tpause\t\tPauses the simulation.\n"
"\texit\t\tClose application without saving.\n"
"\nSYNOPSIS offline\n"
"\t<exec> [params]\n"
"OPTIONS\n"
"\t-i|--iterations=<X>\tSet number of iterations for simulation (default=1).\n"
"\t-o|--output=<filename>\tSet name of output file (default=output.lif).\n"
"\t<filename>\t\tSet name of input file.\n";

enum class GW_state { start, next };

static int32_t gw_index;

static std::string get_word(const char* buffer, GW_state signal)
{
	std::string word;
	if (signal == GW_state::start)
	{
		gw_index = 0;
	}

	while (buffer[gw_index] != '\0')
	{
		// skip whitespaces
		if (isspace(buffer[gw_index])) while (isspace(buffer[gw_index])) gw_index++;

		while (!isspace(buffer[gw_index]) && buffer[gw_index] != '\0')
		{
			word += buffer[gw_index++];
		}
		return word;
	}
	return word;
}

int32_t ConsoleHandler::pos_at_line = 0;
int32_t ConsoleHandler::w8_key = -1;

ConsoleHandler::ConsoleHandler()
{
	this->state = ConsoleStates::working;
	this->shared_data = malloc(256);
};

ConsoleHandler::~ConsoleHandler()
{
	free(this->shared_data);
}

void ConsoleHandler::ask()
{
	int32_t key;
	bool res;
	__pressed_key__(key, res);
	if (res)
	{
		if (w8_key == key)
		{
			w8_key = -1;
			state = ConsoleStates::working;
			pos_at_line = 0;
			stream[pos_at_line] = '\0';
			err_msg.clear();
			err_msg.str("");
			return;
		}

		switch (key)
		{
			//backspace
		case Keys::backspace:
		{
			if (pos_at_line == 0) return;
			pos_at_line--;
			stream[pos_at_line] = '\0';
			break;
		}
		// enter
		case Keys::enter:
		{
			exec();
			pos_at_line = 0;
			stream[pos_at_line] = '\0';
			break;
		}
		default:
		{
			if (pos_at_line == 255) return;
			stream[pos_at_line++] = key;
			stream[pos_at_line] = '\0';
			break;
		}
		}
	}
}

void ConsoleHandler::exec()
{
	std::string command = get_word(stream, GW_state::start);
	const char* end = " Press enter to continue...";

	// to do save to file the universe
	if (command == "dump")
	{
		command = get_word(stream, GW_state::next);
		if (command.empty())
		{
			err_msg << "There is no output file. Aborted." << end;
			w8_key = Keys::enter;
			return;
		}
		if (command.size() >= 255)
		{
			err_msg << "The file name is larger than the allowed size." << end;
			w8_key = Keys::enter;
			return;
		}

		err_msg << "The universe is saved to the " << command << "." << end;
		this->write_to_void(this->shared_data, (void*)command.c_str(), command.size() + 1);
		this->state = ConsoleStates::dumping;
		w8_key = Keys::enter;
		return;

	}

	if (command == "tick" || command == "t")
	{
		command = get_word(stream, GW_state::next);
		if (command.empty())
		{
			err_msg << "Enter a number after 'tick|t'." << end;
			w8_key = Keys::enter;
			return;
		}

		int32_t res = atoi(command.c_str());
		if (res == 0)
		{
			err_msg << "Second argument must be a number." << end;
			w8_key = Keys::enter;
			return;
		}
		if (res < 0)
		{
			err_msg << "Second argument must be positive" << end;
			w8_key = Keys::enter;
			return;
		}

		write_to_void(shared_data, &res, sizeof(int));
		this->state = ConsoleStates::changing_tick;
		return;
	}

	if (command == "delay" || command == "d")
	{
		command = get_word(stream, GW_state::next);
		if (command.empty())
		{
			err_msg << "Enter a number after 'delay|t'." << end;
			w8_key = Keys::enter;
			return;
		}

		float res = (float)atof(command.c_str());
		if (res == 0)
		{
			err_msg << "Second argument must be a float number." << end;
			w8_key = Keys::enter;
			return;
		}
		if (res < 0)
		{
			err_msg << "Second argument must be positive" << end;
			w8_key = Keys::enter;
			return;
		}

		write_to_void(shared_data, &res, sizeof(float));
		this->state = ConsoleStates::changing_delay;
		return;
	}

	if (command == "exit")
	{
		this->state = ConsoleStates::exiting;
		return;
	}

	if (command == "pause")
	{
		this->state = ConsoleStates::paused;
		err_msg << end;
		w8_key = Keys::enter;
		return;
	}

	if (command == "help")
	{
		this->state = ConsoleStates::show_help;
		err_msg << help_info << end << std::flush;
		w8_key = Keys::enter;
		return;
	}

	err_msg << "Type help to give more information." << end;
	w8_key = Keys::enter;
	return;
}

std::string ConsoleHandler::what()
{
	return this->err_msg.str();
}

void* ConsoleHandler::get_shared_data()
{
	return shared_data;
}

void ConsoleHandler::write_to_void(void* dst, void* src, size_t size)
{
	unsigned char* ptr_d = (unsigned char*)dst;
	unsigned char* ptr_s = (unsigned char*)src;
	for (unsigned char i = 0; i < size; i++)
	{
		ptr_d[i] = ptr_s[i];
	}
}

