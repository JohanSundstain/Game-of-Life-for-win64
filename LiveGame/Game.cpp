#include <iostream>
#include <vector>

#include "Game.h"


Game::Game(Config& config) 
{
	// Cretating handlers
	hVisible = GetStdHandle(STD_OUTPUT_HANDLE);
	hHidden = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0,
		NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	// read config
	conf = config;

	// read current buffer size
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hVisible, &csbi);
	wBuff = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	hBuff = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	w = MAX_FIELD_WIDTH;
	h = MAX_FIELD_HEIGHT;

	COORD bufferSize = { MAX_FIELD_WIDTH, MAX_FIELD_HEIGHT };
	SetConsoleScreenBufferSize(hVisible, bufferSize);
	SetConsoleScreenBufferSize(hHidden, bufferSize);

	SMALL_RECT windowSize = { 0, 0, wBuff - 1, hBuff - 1 };
	SetConsoleWindowInfo(hVisible, TRUE, &windowSize);
	SetConsoleWindowInfo(hHidden, TRUE, &windowSize);

	// disable cursor 
	CONSOLE_CURSOR_INFO cci;
	GetConsoleCursorInfo(hVisible, &cci);
	cci.bVisible = FALSE;
	SetConsoleCursorInfo(hVisible, &cci);
	GetConsoleCursorInfo(hHidden, &cci);
	cci.bVisible = FALSE;
	SetConsoleCursorInfo(hHidden, &cci);
	
	imageBuffer.resize(MAX_FIELD_WIDTH * MAX_FIELD_HEIGHT); // +2 for boarder up, down, left, right
	field.resize(MAX_FIELD_WIDTH * MAX_FIELD_HEIGHT);

	SetStartField();
	SetConsoleActiveScreenBuffer(hVisible);
};

Game::~Game() 
{
	CloseHandle(hHidden);
};

void Game::Run()
{
	Draw(); // To change screen buffers. Next itaration will show image. So we can see game from start
	while (state != States::EXIT)
	{
		if (GetAsyncKeyState(VK_LEFT) & 0x8000) viewportX--;
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000) viewportX++;
		if (GetAsyncKeyState(VK_UP) & 0x8000) viewportY--;
		if (GetAsyncKeyState(VK_DOWN) & 0x8000) viewportY++;

		viewportX = max(0, min(viewportX, MAX_FIELD_WIDTH - wBuff));
		viewportY = max(0, min(viewportY, MAX_FIELD_HEIGHT - hBuff));

		SMALL_RECT windowPos = 
		{
			(SHORT)viewportX,
			(SHORT)viewportY,
			(SHORT)(viewportX + wBuff - 1),
			(SHORT)(viewportY + hBuff - 1)
		};
		SetConsoleWindowInfo(hVisible, TRUE, &windowPos);
		SetConsoleWindowInfo(hHidden, TRUE, &windowPos);

		Draw();
		Update();
		Sleep(50);
	}; 
};

void Game::Update()
{
	std::vector<Cell> nextGen = field; 
	for (INT i = 0; i < h; i++)
	{
		for (INT j = 0; j < w; j++)
		{
			INT ind = i * w + j;
			BYTE neighbors = CountNeighbors(j, i);
			if (field[ind].isAlive) nextGen[ind].isAlive = CheckToSurvive(neighbors);
			else nextGen[ind].isAlive = CheckToBirth(neighbors);
		}
	}
	field = nextGen;
}

void Game::Draw()
{
	LoadImageToBuffer();          
	WriteToScreenBuffer(hHidden); 

	SetConsoleActiveScreenBuffer(hHidden);

	std::swap(hVisible, hHidden);
}

void Game::WriteToScreenBuffer(HANDLE hBuffer)
{
	SHORT x = 0, y = 0;
	COORD bufferSize = { MAX_FIELD_WIDTH, MAX_FIELD_HEIGHT };
	COORD bufferCoord = { (SHORT)viewportX, (SHORT)viewportY};
	SMALL_RECT writeRegion = { x, y, x + wBuff - 1, hBuff - 1 };
	WriteConsoleOutputW(hBuffer, imageBuffer.data(), bufferSize, bufferCoord, &writeRegion);
}

void Game::LoadImageToBuffer()
{

	for (INT i = viewportY; i < viewportY + hBuff; i++)
	{
		for (INT j = viewportX; j < viewportX + wBuff; j++)
		{
			INT max_w = MAX_FIELD_WIDTH;
			INT max_h = MAX_FIELD_HEIGHT;

			INT ind = i * max_w + j;

			imageBuffer[ind].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

			if (i == 0 && j == 0)							imageBuffer[ind].Char.UnicodeChar = L'┌';
			else if (i == 0 && j == (max_w - 1))			imageBuffer[ind].Char.UnicodeChar = L'┐';
			else if (i == (max_h - 1) && j == 0)			imageBuffer[ind].Char.UnicodeChar = L'└';
			else if (i == (max_h - 1) && j == (max_w - 1))	imageBuffer[ind].Char.UnicodeChar = L'┘';
			else if (j == 0 || j == (max_w - 1))			imageBuffer[ind].Char.UnicodeChar = L'│';
			else if (i == 0 || i == (max_h - 1))			imageBuffer[ind].Char.UnicodeChar = L'─';
			else
			{
				INT fieldIndex = (i - 1) * max_w + (j - 1);
				if (field[fieldIndex].isAlive)
				{
					imageBuffer[ind].Char.UnicodeChar = L'೦';
				}
				else
				{
					imageBuffer[ind].Char.UnicodeChar = L' ';
				}
			}

		}
	}
}

void Game::SetStartField()
{
	if (conf.offset)
	{
		NormalizeCoord();
	}

	for (auto& p : conf.coordinates)
	{
		INT ind = p.y * w + p.x;
		field[ind].isAlive = 1;
	}
}

void Game::NormalizeCoord()
{
	std::pair<INT, INT> center = {MAX_FIELD_WIDTH / 2, MAX_FIELD_HEIGHT / 2 };

	///////////
	// 0 1 2 x
	// 1
	// 2
	// y

	for (auto& p : conf.coordinates)
	{
		

		p.x = center.first + p.x;
		p.y = center.second - p.y;
	}
}

BYTE Game::CountNeighbors(INT x, INT y)
{
	BYTE neighbors = 0;
	for (INT i = -1; i <= 1; i++) {
		for (INT j = -1; j <= 1; j++) {
			if (i == 0 && j == 0) continue;

			// Временные координаты
			INT curX = x + j;
			INT curY = y + i;

			// Тороидальное поле (зацикливание)
			if (curX < 0) curX = MAX_FIELD_WIDTH - 1; else if (curX >= MAX_FIELD_WIDTH) curX = 0;
			if (curY < 0) curY = MAX_FIELD_HEIGHT - 1; else if (curY >= MAX_FIELD_HEIGHT) curY = 0;

			if (field[curY * w + curX].isAlive) neighbors++;
		}
	}
	return neighbors;
}

BOOL Game::CheckToBirth(BYTE neighbors)
{
	for (INT i = 0; i < conf.rules.B.size(); i++)
	{
		if (conf.rules.B[i] == neighbors) return true;
	}
	return false;
}

BOOL Game::CheckToSurvive(BYTE neighbors)
{
	for (INT i = 0; i < conf.rules.S.size(); i++)
	{
		if (conf.rules.S[i] == neighbors) return true;
	}
	return false;
}

