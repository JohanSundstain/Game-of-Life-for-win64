#pragma once

#include <string>
#include <vector>

typedef struct Rules
{
	unsigned char B[9];
	unsigned char S[9];
};

typedef struct FileFormat
{
	Rules rules;
	std::string name_of_universe;
	std::string rules_str;
	std::string header;
	std::vector<std::pair<int32_t, int32_t>> coordinates;
	int32_t width, height;
	bool offset;
};


