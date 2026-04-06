#include "Field.h"
#include "UsersExceptions.h"
#include <iostream>
#include <vector>
#include <sstream>

int32_t Field::count = 0;

Field::Field() {};

Field::Field(int32_t width, int32_t height)
{
	this->pholder_ws = ' ';
	this->pholder_cl = 'o';
	this->width = width;
	this->height = height;
	std::vector<std::vector<bool>> tmp(height, std::vector<bool>(width, false));
	this->field = std::move(tmp);
};

Field::~Field()
{
	this->pholder_ws = ' ';
	this->pholder_cl = 'o';
};

Field Field::create_field(int32_t width, int32_t height)
{
	if (count == 0)
	{
		count++;
		return Field(width, height);
	}
	else
	{
		throw ClassError("The object of class Field must to be only one\n");
	}
}

Field* Field::create_field_ptr(int32_t width, int32_t height)
{
	if (count == 0)
	{
		count++;
		return new Field(width, height);
	}
	else
	{
		throw ClassError(static_cast<std::string>("The object of class Field must to be only one\n"));
	}
}

int32_t Field::count_neighbors(int32_t x, int32_t y)
{
	int32_t neighbors = 0;
	if (x < 0 || y < 0) throw IndexError(x, y);

	// first layer 
	// [x-1, y-1] [x, y-1] [x+1, y-1]
	neighbors += (*this)(x - 1, y - 1) ? 1 : 0;
	neighbors += (*this)(x, y - 1) ? 1 : 0;
	neighbors += (*this)(x + 1, y - 1) ? 1 : 0;

	// second layer 
	// [x-1, y] [x,y] [x+1, y]
	neighbors += (*this)(x - 1, y) ? 1 : 0;
	neighbors += (*this)(x + 1, y) ? 1 : 0;

	// third layer
	// [x-1, y+1] [x, y+1] [x+1, y+1]
	neighbors += (*this)(x - 1, y + 1) ? 1 : 0;
	neighbors += (*this)(x, y + 1) ? 1 : 0;
	neighbors += (*this)(x + 1, y + 1) ? 1 : 0;

	return neighbors;
}

int32_t Field::get_width() const
{
	return this->width;
}

int32_t Field::get_height() const
{
	return this->height;
}

void Field::set_pholder_ws(const char c)
{
	this->pholder_ws = c;
}

void Field::set_pholder_cl(const char c)
{
	this->pholder_cl = c;
}

std::vector<std::pair<int32_t, int32_t>> Field::get_filled_cells()
{
	int32_t w = this->get_width();
	int32_t h = this->get_height();

	std::vector<std::pair<int32_t, int32_t>> xy;

	for (int32_t y = 0; y < h; y++)
	{
		for (int32_t x = 0; x < w; x++)
		{
			if ((*this)[y][x])
			{
				xy.push_back({ x,y });
			}
		}
	}

	return xy;
}

std::string  Field::get_string_view()
{
	std::stringstream picture;

	int32_t w = this->get_width();
	int32_t h = this->get_height();

	for (int32_t y = 0; y < h; y++)
	{
		for (int32_t x = 0; x < w; x++)
		{
			picture << (this->field[y][x]
				? this->pholder_cl
				: this->pholder_ws);
		}
		picture << std::endl;
	}
	return picture.str();
}

//operators
std::vector<bool>& Field::operator[](int32_t row)
{
	return this->field[row];
}

bool Field::operator()(int32_t x, int32_t y)
{
	int32_t real_x;
	int32_t real_y;
	if (x == -1)				real_x = this->width - 1;
	else if (x == this->width)	real_x = 0;
	else						real_x = x;
	if (y == -1)				real_y = this->height - 1;
	else if (y == this->height)	real_y = 0;
	else						real_y = y;

	return (*this)[real_y][real_x];
}

std::ostream& operator<<(std::ostream& stream, const Field& f)
{
	std::stringstream picture;

	int32_t w = f.get_width();
	int32_t h = f.get_height();

	for (int32_t y = 0; y < h; y++)
	{
		for (int32_t x = 0; x < w; x++)
		{
			picture << (f.field[y][x]
				? f.pholder_cl
				: f.pholder_ws);
		}
		picture << std::endl;
	}
	stream << picture.str() << std::endl << std::flush;
	return stream;
}