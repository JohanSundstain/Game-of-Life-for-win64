#pragma once

#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <functional>

enum class Mode {offline, online};

class Initializer
{
private:
	Mode mode;
	std::ofstream fout;
	std::ifstream fin;
	std::map<std::string, std::function<void(std::string,Initializer&)>> args_func;
	int32_t iterations;

	// input file, -i|--iterations=x, -o|--output=filename
	void parse_args(int32_t argc, char** argv);
public:
	Initializer(int32_t argc, char** argv);

	~Initializer();

	// function with main initialization
	void init();

	friend void set_ifilename(std::string name, Initializer& init);

	friend void set_ofilename(std::string name, Initializer& init);

	friend void set_iterations(std::string value, Initializer& init);

};
