#include "FileRW.h"
#include "FileFormat.h"
#include "UsersExceptions.h"
#include <fstream>
#include <sstream>

FileRW::FileRW() {};

void FileRW::set_ifile(std::ifstream& fin)
{
	this->fin = &fin;
}

void FileRW::set_ofile(std::ofstream& fout)
{
	this->fout = &fout;
}

void FileRW::read_ifile()
{
	char buff[256];
	fin->getline(buff, 256);
	while (read_iparams(buff)) fin->getline(buff, 256);

	while (!fin->eof())
	{
		read_icoordinates(buff);
		fin->getline(buff, 256);
	}
	normalize_coordinates();

}

bool FileRW::read_iparams(std::string line)
{
	if (line.front() != '#') return false;

	transformer.clear();
	transformer.str("");
	transformer << line;
	std::string word;

	transformer >> word;

	if (word == "#Life")
	{
		transformer >> word;
		this->read_file.header = word;
		return true;
	}

	if (word == "#N")
	{
		// skip #N
		for (int32_t i = 3; i < line.size(); i++)
		{
			this->read_file.name_of_universe += line[i] ;
		}

		return true;
	}

	if (word == "#R")
	{
		transformer >> word;
		this->read_file.rules_str = word;
		parse_rules(word);
		return true;
	}
	// #S W/H
	if (word == "#S")
	{
		char separator;
		transformer >> this->read_file.width >> separator >> this->read_file.height;
		if (separator != '/')
		{
			throw ArgsError("Invalid separator in #S instruction: <Width>/<Height>\n");
		}
		return true;
	}

	return false;
}

void FileRW::read_icoordinates(std::string line)
{
	if (line.empty()) return;

	transformer.clear();
	transformer.str("");
	transformer << line;

	std::pair<int32_t, int32_t> xy;

	transformer >> xy.first >> xy.second;
	if (!this->read_file.offset && (xy.first < 0 || xy.second < 0)) this->read_file.offset = true;

	this->read_file.coordinates.push_back(xy);
}

void FileRW::parse_rules(std::string line)
{
	int32_t len = line.length();
	const char* start = "Invalid rule format: ";
	std::stringstream err_msg;

	if (tolower(line.front()) != 'b')
	{
		err_msg << start << "B[{0..8}]/S[{0..8}]>" << std::endl;
		throw ArgsError(err_msg.str());
	}

	unsigned char B_index = 0, S_index = 0;

	int32_t num, i = 1;
	for (; i < len && line[i] != '/'; i++)
	{
		if (!isdigit(line[i]))
		{
			err_msg << start << "'" << line[i] << "'" << " must be a number" << std::endl;
			throw ArgsError(err_msg.str());
		}

		if ((unsigned char)(line[i] - '0') > (unsigned char)8)
		{
			err_msg << start << (int)(line[i] - '0') << " does not belong to [0..8]" << std::endl;
			throw ArgsError(err_msg.str());
		}

		this->read_file.rules.B[B_index++] = (unsigned char)(line[i] - '0');
	}

	if (B_index != 9 && B_index != 0) this->read_file.rules.B[B_index] = 255;

	if (i >= len - 1)
	{
		err_msg << start << " There is no S definition" << std::endl;
		throw ArgsError(err_msg.str());
	}

	i += 2; // skip '/' and S
	for (; i < len && S_index < 9; i++)
	{
		if (!isdigit(line[i]))
		{
			err_msg << start << "'" << line[i] << "'" << " must be a number" << std::endl;
			throw ArgsError(err_msg.str());
		}
		if ((unsigned char)(line[i] - '0') > (unsigned char)8)
		{
			err_msg << start << (int)(line[i] - '0') << " does not belong to [0..8]" << std::endl;
			throw ArgsError(err_msg.str());
		}

		this->read_file.rules.S[S_index++] = (unsigned char)(line[i] - '0');
	}

	if (S_index != 9 && S_index != 0) this->read_file.rules.S[S_index] = 255;

}

void FileRW::write_ofile(const std::vector<std::pair<int32_t, int32_t>>& xy)
{
	transformer.clear();
	transformer.str("");

	// first line
	transformer << "#Life 1.06" << std::endl;

	// second line
	if (read_file.name_of_universe.empty())
		read_file.name_of_universe = "nameless";

	transformer << "#N " << read_file.name_of_universe << std::endl;

	// third line
	transformer << "#R " << read_file.rules_str << std::endl;

	// forth line
	transformer << "#S " << read_file.width << "/" << read_file.height << std::endl;


	// coordinates
	for (auto p : xy)
	{
		transformer << p.first << " " << p.second << std::endl;
	}

	fout->write(transformer.str().c_str(), transformer.str().length());

}

void FileRW::normalize_coordinates()
{
	if (!read_file.offset) return;

	std::pair<int32_t, int32_t> center = { read_file.width / 2, read_file.height / 2 };

	//////////////////
	// O x 0 1 2 . . .
	// y
	// 0
	// 1
	// 2
	// .
	// .
	// .

	for (auto& p : read_file.coordinates)
	{
		p.first = center.first + p.first;
		p.second = center.second - p.second;
	}
}