#include <iostream>
#include <sstream>
#include "UsersExceptions.h"

std::string Error::what()
{
	return this->err_msg;
};

IndexError::IndexError(int32_t x, int32_t y) : x(x), y(y) 
{
	std::stringstream error_msg;
	error_msg << "Incorrect index: " << this->x << " " << this->y << std::endl;
	this->err_msg = error_msg.str();
};

IndexError::IndexError(std::string msg)
{
	this->err_msg = msg;
};

ClassError::ClassError(std::string msg)
{
	this->err_msg = msg;
};

ArgsError::ArgsError(std::string msg)
{
	this->err_msg = msg;
};