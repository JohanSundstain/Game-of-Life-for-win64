#pragma once

#include <stdint.h>
#include <string>

class Error
{
protected:
	std::string err_msg;
public:
	std::string what();
};

class IndexError : public Error
{
private:
	int32_t x,y;
public:
	IndexError(int32_t x, int32_t y);
	IndexError(std::string msg);
};

class ClassError : public Error
{
public:
	ClassError(std::string msg);

};

class ArgsError : public Error
{
public:
	ArgsError(std::string msg);

};
