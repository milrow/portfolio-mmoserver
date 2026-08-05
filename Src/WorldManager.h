#pragma once

#include <vector>

using namespace std;

struct Cell
{
	int32_t x;
	int32_t y;
};
using WorldGrid = vector<Cell>;

class WorldManager
{
private:


public:
	static WorldManager& GetInstance() 
	{
		static WorldManager _instance;
		return _instance;
	}

	void Update();
};

