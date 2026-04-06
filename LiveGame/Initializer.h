#pragma once

#include <vector>
#include <map>
#include <string>
#include <functional>

#include "GameTypes.h"

class Initializer
{
private:
	std::vector<std::string> defaultFiles = 
	{ "fractal.lif", "glider.lif", "glidergun.lif", "pulsar.lif", "sirrobin.lif" };
	Config conf;
	void ParseFile(const std::string);
public:
	Initializer(INT argc, CHAR** argv);

	~Initializer();
};
