#pragma once

#include "Field.h"
#include "ConsoleHandler.h"
#include "FileRW.h"
#include "FileFormat.h"
#include <list>
#include <sstream>

class Game
{
private:
	std::vector<std::pair<bool, std::pair<int, int>>> buffer;
	std::stringstream picture;
	Rules rules;
	Field* field;
	ConsoleHandler* chandler;
	FileRW* frw;
	int32_t tick;
	size_t count_of_ticks;
	bool game_over;
	float delay;
	void update(int32_t tick);
	inline void draw();

	bool check_to_birth(int32_t neighbors);
	bool check_to_survive(int32_t neighbors);
public:
	Game();
	Game(Field& f, ConsoleHandler& c, FileRW& frw, Rules& r);
	~Game();
	// start the game
	void run();
	void set_start_field(std::vector<std::pair<int32_t, int32_t>> xy);
	void calculate_state(int32_t tick);
};


