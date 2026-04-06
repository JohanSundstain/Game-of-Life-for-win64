#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include "FileFormat.h"

class FileRW
{
private:
	std::ifstream* fin;
	std::ofstream* fout;
	std::stringstream transformer;

	bool read_iparams(std::string line);
	void read_icoordinates(std::string line);
	void parse_rules(std::string line);
	
public:
	FileFormat read_file;

	FileRW();

	void set_ifile(std::ifstream& f);

	void set_ofile(std::ofstream& f);

	void read_ifile();

	void write_ofile(const std::vector<std::pair<int32_t, int32_t>>& xy);

	void normalize_coordinates();

};
