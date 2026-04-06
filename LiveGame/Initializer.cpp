#include <vector>
#include <functional>
#include <sstream>
#include <fstream>

#include "Initializer.h"
#include "UsersExceptions.h"
#include "Game.h"
#include "Field.h"
#include "FileRW.h"

void set_ofilename(std::string name, Initializer& init)
{
	init.fout.open(name.c_str());

	if (init.fout.fail())
	{
		throw ArgsError("Something went wrong");
	}

	return;
}

void set_ifilename(std::string name, Initializer& init)
{
	init.fin.open(name.c_str());
	std::stringstream err_msg;

	if (init.fin.fail())
	{
		err_msg << "There is no such file: " << name;
		throw ArgsError(err_msg.str());
	}
	
	return;
}

void set_iterations(std::string name, Initializer& init)
{
	int32_t res = atoi(name.c_str());
	if (res <= 0)
	{
		throw ArgsError("Iteration's value must be positive number and greater than 1");
	}
	init.iterations = res;

	return;
}

static std::pair<std::string, std::string> separate_long_arg(const std::string& arg)
{
	std::pair<std::string, std::string> res;

	int32_t i, len = arg.size();

	for (i = 0; arg[i] != '=' && i < len; i++)
	{
		if (isspace(arg[i])) continue;
		res.first += arg[i];
	}
	// there is no '=' => it is mistake 
	if (i >= len - 1)
	{
		throw ArgsError("There is no value");
	}

	i++;
	for (; i < len; i++)
	{
		res.second += arg[i];
	}

	return res;
}

Initializer::Initializer(int32_t argc, char** argv)
{
	this->args_func["-o"] = set_ofilename;
	this->args_func["--output"] = set_ofilename;
	this->args_func["-i"] = set_iterations;
	this->args_func["--iterations"] = set_iterations;
	this->args_func["input"] = set_ifilename;
	iterations = 1;
	this->parse_args(argc, argv);

	if (!fout.is_open())
		fout.open("output.lif");

	if (!fin.is_open())	mode = Mode::online;
	else				mode = Mode::offline;
}

Initializer::~Initializer()
{
	fin.close();
	fout.close();
};

// input file, -i|--iterations=x, -o|--output=filename
void Initializer::parse_args(int32_t argc, char** argv)
{
	if (argc == 1)
	{
		mode = Mode::online;
		return;
	}

	for (int32_t i = 1; i < argc; i++)
	{	
		std::stringstream err_msg;
		// ong arg
		std::pair<std::string, std::string> separated_arg;
		if (argv[i][0] == '-' && argv[i][1] == '-')
		{
			separated_arg = separate_long_arg(argv[i]);

			if (args_func.find(separated_arg.first) == args_func.end())
			{
				err_msg << "There is no such argument: " << separated_arg.first;
				throw ArgsError(err_msg.str());
			}
			args_func[separated_arg.first](separated_arg.second, *this);
		}
		// short arg
		else if (argv[i][0] == '-')
		{
			if (args_func.find(argv[i]) == args_func.end())
			{
				err_msg << "There is no such argument: " << argv[i];
				throw ArgsError(err_msg.str());
			}
			if (i == argc - 1)
			{
				err_msg << "Missing value after " << argv[i];
				throw ArgsError(err_msg.str());
			}
			args_func[argv[i]](argv[i + 1], *this);
			i++;
		}
		// input file
		else
		{
			args_func["input"](argv[i], *this);
		}
	}
}

void Initializer::init()
{
	// online = there is no input file
	FileRW* frw = nullptr;
	Field* field = nullptr;
	Game* game = nullptr;
	ConsoleHandler* chandler = nullptr;

	if (mode == Mode::online)
	{
		srand(time(NULL));
		std::stringstream err_msg;
		std::vector<std::string> default_files =
		{ "fractal.lif", "glider.lif", "glidergun.lif", "pulsar.lif", "sirrobin.lif" };

#if defined(_WIN64)
		std::string pref = ".\\files\\";
#elif defined __unix__ || defined(__gnu_linux__) || defined(__linux__)
		std::string pref = "./files/"
#endif

		this->fin.open(pref+default_files[rand() % default_files.size()]);
		if (this->fin.fail())
		{
			err_msg << "There is no such file: " << "file.txt";
			throw ArgsError(err_msg.str());
		}
		// read file;
		frw = new FileRW();
		frw->set_ifile(fin);
		frw->read_ifile();
		// create needed field 
		field = Field::create_field_ptr(frw->read_file.width, frw->read_file.height);
		//create a console handler
		chandler = new ConsoleHandler();
		// create a Game session
		game = new Game(*field, *chandler,*frw, frw->read_file.rules);
		game->set_start_field(frw->read_file.coordinates);
		game->run();
	}
	// else offline regime
	else
	{
		// read file;
		frw = new FileRW();
		frw->set_ifile(fin);
		frw->set_ofile(fout);
		frw->read_ifile();
		// create field
		field = Field::create_field_ptr(frw->read_file.width, frw->read_file.width);
		//create a console handler
		chandler = new ConsoleHandler();
		// start game
		game = new Game(*field, *chandler, *frw, frw->read_file.rules);
		game->set_start_field(frw->read_file.coordinates);
		game->calculate_state(this->iterations);
		// writing universe
		frw->write_ofile(field->get_filled_cells());
	}

	delete frw;
	delete field;
	delete game;
	delete chandler;
}
