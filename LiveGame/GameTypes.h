#pragma once
#include <Windows.h>
#include <cstdint>
#include <vector>

#define DEFAULT_WIDHT 25
#define DEFAULT_HEIGHT 25
#define MIN_WIDTH 5
#define MIN_HEIGHT 5

#define MAX_FIELD_WIDTH 120
#define MAX_FIELD_HEIGHT 35


struct Cell
{
	BOOL isAlive = 0;
};

struct Point
{
	INT x;
	INT y;
};

struct Rules
{
	std::vector<BYTE> B{ 3 };
	std::vector<BYTE> S{ 2, 3 };
};

struct Config
{
	std::string header = "#Life 1.06";
	std::string nameOfUniverse = "Unknown";
	Rules rules;
	DWORD width = DEFAULT_WIDHT;
	DWORD height = DEFAULT_HEIGHT;
	std::vector<Point> coordinates;
	BOOL offset = FALSE;
};