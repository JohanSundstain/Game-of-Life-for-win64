#pragma once
#include <Windows.h>

struct Cell
{
	DWORD x = 0;
	DWORD y = 0;
	BOOL isAlive = false;
};

struct Rules
{
	WCHAR B[9];
	WCHAR S[9];
};

struct FileFormat
{
	Rules rules;
	std::string name_of_universe;
	std::string rules_str;
	std::string header;
	std::vector<std::pair<int32_t, int32_t>> coordinates;
	int32_t width, height;
	bool offset;
};