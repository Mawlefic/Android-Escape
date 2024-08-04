#pragma once
#include "collision_object.h"
#include "raylib.h"
#include "entity.h"
#include <vector>

class Tile;
class TileMap;
class MovingTile;

const int TILE_SIZE = 20;

struct Cell {
	int x, y;
};

class TileMap {
private:
	const int DEFAULT_WIDTH = 50;
	const int DEFAULT_HEIGHT = 30;

public:
	// attributes
	std::vector<std::vector<Tile>> tiles;
	int yTiles = 0;
	int xTiles = 0;
	float mapWidth = 0;
	float mapHeight = 0;

	// functions
	TileMap();
	void Init(const char* mapPath, int& playerX, int& playerY);
	void Reset();
	void EnableCell(int x, int y);
	void DisableCell(int x, int y);
	// out of bounds tiles always false
	bool isTileEnabled(int x, int y);

};

class Tile {
protected:

public:
	// functions
	Tile();
	Tile& operator=(const Tile& t);
	void Draw(int cellX, int cellY);

	// attributes
	bool enabled;
};

class Gate {
private:
	TileMap& tm;

public:
	bool enabled;
	std::vector<Cell> cells;

	// functions
	Gate(TileMap& tilemap);
	void Activate();
	void Deactivate();
};

class MovingTile : public Entity {
private:
	// functions
	void UpdateTimer(float delta);
	void MoveX(float amount);
	void MoveY(float amount);

	// attributes
	bool direction;
	float lerpSpeed, lerpTime, lerpValue;
	float waitInterval, waitValue;
	float xRemainder, yRemainder;

public:
	// functions
	MovingTile();
	MovingTile(int x, int y);
	MovingTile& operator=(MovingTile& mt);
	void Update(float delta) override;
	void Draw() override;
	void SetMoveParameters(int endX, int endY, float time, float waitTime);
	void SetOffsetParameters(int offsetX, int offsetY, float time, float waitTime);
	void Init(int x, int y);

	// atrributes
	CollisionComponent cc;
	bool enabled, isWaiting;
	int startX, endX, startY, endY, moveX, moveY;
};