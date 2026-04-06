#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdint.h>
#include <sstream>

#if defined(_WIN64)

#include <conio.h>
#include <stdio.h>

enum Keys { enter = 13, backspace = 8 };

#define __cons_flush__(str, ... )	\
printf(str, __VA_ARGS__);			\
fflush(stdout);						\


#define __pressed_key__(key, res)	\
if (_kbhit())						\
{									\
	key = (int32_t)_getch();		\
	res =  true;					\
}									\
else res = false;					\


#elif defined __unix__ || defined(__gnu_linux__) || defined(__linux__)
#include <curses.h>

enum Keys { enter = 10, backspace = 127 };

#define __cons_flush__(str, ...)	\
printw(str, __VA_ARGS__);			\
refresh();							\

#define __pressed_key__(key, res)	\
halfdelay(1);						\
key = getch();						\
if (key != ERR)						\
{									\
	res = true;						\
}									\
else res = false;					\


#endif