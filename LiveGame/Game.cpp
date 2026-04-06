#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>

#include "Game.h"
#include "UsersExceptions.h"
#include "KeyListener.h"


Game::Game(Field& f, ConsoleHandler& c, FileRW& frw, Rules& r)
{
	this->rules = std::move(r);
	this->frw = &frw;
	this->field = &f;
	this->chandler = &c;
	this->delay = 0.3;
	this->tick = 1;
	game_over = false;
};

Game::Game() {};

Game::~Game() {};

void Game::run()
{
	this->count_of_ticks = 0;
	while (!this->game_over)
	{

		chandler->ask();
		this->draw();

		if (chandler->state == ConsoleStates::dumping)
		{
			std::string filename = (char*)chandler->get_shared_data();
			std::ofstream fout(filename.c_str());
			this->frw->set_ofile(fout);
			this->frw->write_ofile(this->field->get_filled_cells());
			fout.close();
			chandler->state = ConsoleStates::working;
		}

		if (chandler->state == ConsoleStates::exiting)
			game_over = true;

		if (chandler->state == ConsoleStates::changing_tick)
		{
			tick = *(int*)chandler->get_shared_data();
			chandler->state = ConsoleStates::working;
		}

		if (chandler->state == ConsoleStates::changing_delay)
		{
			delay = *(float*)chandler->get_shared_data();
			chandler->state = ConsoleStates::working;
		}

		this->update(chandler->state == ConsoleStates::paused ? 0 : tick);
		count_of_ticks += chandler->state == ConsoleStates::paused ? 0 : tick;
	}
	cclr();
#if defined(__unix__) || defined(__gnu_linux__) || defined(__linux__)
	endwin();
#endif
};

inline void Game::draw()
{
	cclr();
	__cons_flush__(">%s\n", chandler->stream);
	__cons_flush__("%s\t%s\t%d\n",
		frw->read_file.name_of_universe.c_str(),
		frw->read_file.rules_str.c_str(),
		this->count_of_ticks);
	__cons_flush__("State: %s\n", chandler->what().c_str());
	if (chandler->state != ConsoleStates::show_help)
		__cons_flush__("%s\n", field->get_string_view().c_str());
	slp(delay);
}

void Game::set_start_field(std::vector<std::pair<int32_t, int32_t>> xy)
{
	int32_t w = field->get_width();
	int32_t h = field->get_height();

	for (auto p : xy)
	{
		if (p.first >= w || p.second >= h || p.first < 0 || p.second < 0)
		{
			throw IndexError("The index from file goes beyond the bounds of the array");
		}
		(*this->field)[p.second][p.first] = true;
	}
}

void Game::calculate_state(int32_t tick)
{
	update(tick);
}

void Game::update(int32_t tick)
{
	int32_t w = (*field).get_width();
	int32_t h = (*field).get_height();

	for (int32_t time = 0; time < tick; time++)
	{
		for (int32_t y = 0; y < h; y++)
		{
			for (int32_t x = 0; x < w; x++)
			{
				int32_t nei = (*field).count_neighbors(x, y);
				if ((*field)[y][x])	buffer.push_back({ check_to_survive(nei),{x,y} });
				else				buffer.push_back({ check_to_birth(nei),{x,y} });
			}
		}

		for (const auto el : buffer)
		{
			(*field)[el.second.second][el.second.first] = el.first;
		}
		buffer.clear();
	}

}

bool Game::check_to_birth(int32_t neighbors)
{
	for (int32_t i = 0; i < 9 && rules.B[i] != 255; i++)
	{
		if (rules.B[i] == neighbors) return true;
	}
	return false;
}

bool Game::check_to_survive(int32_t neighbors)
{
	for (int32_t i = 0; i < 9 && rules.S[i] != 255; i++)
	{
		if (rules.S[i] == neighbors) return true;
	}
	return false;
}