#pragma once

#include <windows.h>
#include "GameTypes.h"


class Game
{
private:
	enum class States { RUN, PAUSED, EXIT };
	std::vector<CHAR_INFO> imageBuffer;
	std::vector<Cell> field;
	Config conf;

	HANDLE hVisible, hHidden;

	DWORD tick = 0;
	States state = States::RUN;
	SHORT w = DEFAULT_WIDHT, h = DEFAULT_HEIGHT;
	SHORT wBuff = DEFAULT_WIDHT, hBuff = DEFAULT_HEIGHT;
	INT viewportX = 0;  
	INT viewportY = 0; 

	void Update();
	void Draw();
	BYTE CountNeighbors(INT x, INT y);
	BOOL CheckToBirth(BYTE);
	BOOL CheckToSurvive(BYTE);
	void NormalizeCoord();
	void LoadImageToBuffer();
	void WriteToScreenBuffer(HANDLE);
	void SetStartField();

public:
	Game(Config&);
	~Game();
	// start the game
	void Run();
};


