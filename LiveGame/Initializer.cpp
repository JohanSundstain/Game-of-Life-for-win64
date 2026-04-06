#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <ranges>

#include "Initializer.h"
#include "GameTypes.h"
#include "Game.h"
#include "Field.h"


Initializer::Initializer(INT argc, CHAR** argv)
{

	srand(time(NULL));
	std::string path;
	if (argc > 1)
	{
		path = argv[1];
	}
	else
	{
		path = "files/" + defaultFiles[rand() % defaultFiles.size()];
		std::cerr << path << std::endl;
	}
	ParseFile(path);

	Game game(conf);
	game.Run();

}

Initializer::~Initializer() {};

void Initializer::ParseFile(const std::string path)
{
	std::ifstream inputFile(path);
	if (!inputFile.is_open())
	{
		std::cerr << "cannot open " << path << std::endl;
		return;
	}

	std::string buffer;
	
	while (std::getline(inputFile, buffer))
	{
		std::stringstream wordsStream(buffer);
		std::string op, param;
		wordsStream >> op >> param;
		if (op == "#Life")
		{
			if (param != "1.06")
			{
				std::cerr << "string \"#Life 1.06\" must be" << std::endl;
				exit(1);
			}
		}
		else if (op == "#N")
		{
			conf.nameOfUniverse = param;
		}
		else if (op == "#R")
		{
			size_t pos = param.find('/');
			if (pos == std::string::npos)
			{
				std::cerr << "invalid rules line" << std::endl;
				exit(1);
			}
			std::string B = param.substr(0, pos);
			std::string S = param.substr(pos + 1);
			if (S.empty() || B.empty())
			{
				std::cerr << "invalid rules line" << std::endl;
				exit(1);
			}
			/*
			* B/S - no 
			* B<num>/S<num> - yes
			*/
			if ((B.front() != 'B' && B.size() > 1) ||
				(S.front() != 'S' && S.size() > 1))
			{
				std::cerr << "invalid rules line" << std::endl;
				exit(1);
			}

			conf.rules.B.clear();
			conf.rules.S.clear();
			auto fB = B | std::views::filter([](char c) { return ::isdigit(static_cast<int>(c)); });
			auto fS = S | std::views::filter([](char c) { return ::isdigit(static_cast<int>(c)); });
			for (auto b : fB)
			{
				conf.rules.B.push_back(b - '0');
			}
			if (conf.rules.B.size() == 0)
			{
				std::cerr << "invalid rules line" << std::endl;
				exit(1);
			}

			for (auto s : fS)
			{
				conf.rules.S.push_back(s - '0');
			}
			if (conf.rules.S.size() == 0)
			{
				std::cerr << "invalid rules line" << std::endl;
				exit(1);
			}

		}
		else if (op.size() > 0 && param.size() > 0)
		{
			bool isMinus = false;
			for (auto c : op)
			{
				if (c == '-' && !isMinus)
				{
					conf.offset = true;
					isMinus = true;
					continue;
				}
				if (!::isdigit(c))
				{
					std::cerr << "invalid coord line" << std::endl;
					exit(1);
				}
			}
			isMinus = false;
			for (auto c : param)
			{
				if (c == '-' && !isMinus)
				{
					conf.offset = true;
					isMinus = true;
					continue;
				}
				if (!::isdigit(c))
				{
					std::cerr << "invalid coord line" << std::endl;
					exit(1);
				}
			}

			INT x = std::stoi(op);
			INT y = std::stoi(param);
			conf.coordinates.push_back({ x, y });

		}	
	}
}
