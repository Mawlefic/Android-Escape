#include "tile.h"
#include "raymath.h"
#include "../helper/utility.h"
#include <stdio.h>
#include <rlgl.h>

const Color TILE_COLOR = BLACK;
const Color PLAYER_COLOR = Color{ 255, 0, 255 };

// *** TILE ***

Tile::Tile() {
	enabled = false;
}

Tile& Tile::operator=(const Tile& t)
{
	enabled = t.enabled;
	return *this;
}

// Tile(Texture2D tex2D) {}

void Tile::Draw(int cellX, int cellY) {
	if (enabled) {
		DrawRectangle(cellX * TILE_SIZE, cellY * TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_COLOR);
		// draw texture once sprites added
	}
}

// *** TILE MAP ***

TileMap::TileMap() {
	// set default tiles
	tiles.resize(DEFAULT_HEIGHT);
	for (int i = 0; i < tiles.size(); i++) {
		tiles[i].resize(DEFAULT_WIDTH);
	}
	for (int h = 0; h < tiles.size(); h++) {
		for (int w = 0; w < tiles[h].size(); w++) {
			tiles[h][w] = Tile();
			if (h < DEFAULT_HEIGHT / 10) {
				tiles[h][w].enabled = true;
			}
		}
	}
}

void TileMap::Init(const char* mapPath, int& playerX, int& playerY) {
	// load textures
	Image mapImage = LoadImage(mapPath);

	// set tile map dimensions
	xTiles = mapImage.width;
	mapWidth = xTiles;
	yTiles = mapImage.height;
	mapHeight = yTiles;
	tiles.resize(yTiles);
	for (int i = 0; i < tiles.size(); i++) {
		tiles[i].resize(xTiles);
	}
	
	// get map data
	for (int h = 0; h < tiles.size(); h++) {
		for (int w = 0; w < tiles[h].size(); w++) {
			tiles[h][w] = Tile();
			Color iColor = GetImageColor(mapImage, w, h);
			if (ColorEqual(iColor, TILE_COLOR)) {
				tiles[h][w].enabled = true;
			}
			if (ColorEqual(iColor, PLAYER_COLOR)) {
				playerX = w * TILE_SIZE;
				playerY = h * TILE_SIZE;
			}
		}
	}
	UnloadImage(mapImage);
}

void TileMap::Reset() {
	tiles.clear();
}

void TileMap::EnableCell(int x, int y)
{
	tiles[y][x].enabled = true;
}

void TileMap::DisableCell(int x, int y)
{
	tiles[y][x].enabled = false;
}

bool TileMap::isTileEnabled(int x, int y)
{
	if (x < 0 || x >= xTiles || y < 0 || y >= yTiles) {
		return false;
	}
	return tiles[y][x].enabled;
}

// *** MOVING TILE ***

MovingTile::MovingTile()
{
	// position attributes
	cc = CollisionComponent{ 0, 0, TILE_SIZE * 8, 15 };
	cc.Update();
	startX = 0;
	startY = 0;
	endX = startX;
	endY = startY;
	moveX = 0;
	moveY = 0;

	// velocity and movement
	lerpTime = 1.0f;
	lerpSpeed = 1 / lerpTime;
	lerpValue = 0.0f;
	waitInterval = 1.0f;
	waitValue = 0.0f;
	xRemainder = 0.0f;
	yRemainder = 0.0f;

	// bools
	direction = true;
	isWaiting = true;
	enabled = true;
}

MovingTile::MovingTile(int x, int y) : MovingTile()
{
	// position attributes
	cc.x = x;
	cc.y = y;
	cc.Update();
	startX = x;
	startY = y;
	endX = startX;
	endY = startY;
}

MovingTile& MovingTile::operator=(MovingTile& mt)
{
	Init(mt.cc.x, mt.cc.y);
	startX = mt.startX;
	startY = mt.startY;
	endX = mt.endX;
	endY = mt.endY;
	enabled = mt.enabled;
	cc.width = mt.cc.width;
	cc.height = mt.cc.height;
	cc.Update();
	return *this;
}

void MovingTile::Init(int x, int y)
{
	// position attributes
	cc = CollisionComponent{ x, y, TILE_SIZE * 5, TILE_SIZE / 2 };
	cc.Update();
	startX = x;
	startY = y;
	endX = startX;
	endY = startY;
	moveX = 0;
	moveY = 0;

	// velocity and movement attributes
	lerpTime = 1.0f;
	lerpSpeed = 1 / lerpTime;
	lerpValue = 0.0f;
	waitInterval = 1.0f;
	waitValue = 0.0f;
	xRemainder = 0.0f;
	yRemainder = 0.0f;

	// bools
	direction = true;
	isWaiting = true;
	enabled = true;
}

void MovingTile::Update(float delta) {
	if (enabled && !isWaiting) {
		// moving tile update
		if (direction) {
			lerpValue += lerpSpeed * delta;
			if (lerpValue >= 1) {
				lerpValue = 1.0f;
				direction = !direction;
				isWaiting = true;
			}
		}
		else {
			lerpValue -= lerpSpeed * delta;
			if (lerpValue <= 0) {
				lerpValue = 0.0f;
				direction = !direction;
				isWaiting = true;
			}
		}
		Vector2 newPosition = Vector2Lerp(Vector2{ (float)startX, (float)startY }, Vector2{ (float)endX, (float)endY }, lerpValue);
		MoveX(newPosition.x - cc.x);
		MoveY(newPosition.y - cc.y);
	}
	else if (isWaiting) {
		moveX = 0;
		moveY = 0;
		UpdateTimer(delta);
	}
}

void MovingTile::Draw() {
	if (enabled) {
		cc.Draw(DARKPURPLE);
		// draw texture once sprites added
	}
}

void MovingTile::UpdateTimer(float delta) {
	waitValue += delta;
	if (waitValue >= waitInterval) {
		waitValue = 0.0f;
		isWaiting = false;
	}
}

void MovingTile::MoveX(float amount) {
	xRemainder += amount;
	int move = round(xRemainder);
	moveX = move;
	if (move != 0) {
		xRemainder -= move;
		cc.x += move;
		cc.Update();
	}
}

void MovingTile::MoveY(float amount) {
	yRemainder += amount;
	int move = round(yRemainder);
	moveY = move;
	if (move != 0) {
		yRemainder -= move;
		cc.y += move;
		cc.Update();
	}
}

// Any time value <= 0, will be changed to default value 1
void MovingTile::SetMoveParameters(int endX, int endY, float time, float waitTime) {
	this->endX = endX;
	this->endY = endY;
	if (time <= 0) {
		time = 1;
	}
	lerpSpeed = 1 / time;
	waitInterval = waitTime;
}

// Any time value <= 0, will be changed to default value 1
void MovingTile::SetOffsetParameters(int offsetX, int offsetY, float time, float waitTime) {
	endX = startX + offsetX;
	endY = startY + offsetY;
	if (time <= 0) {
		time = 1;
	}
	lerpSpeed = 1 / time;
	waitInterval = waitTime;
}

// *** GATE FUNCTIONS ***

Gate::Gate(TileMap& tilemap) : tm(tilemap)
{
	enabled = false;
}

void Gate::Activate() {
	enabled = true;
	for (int i = 0; i < cells.size(); i++) {
		tm.EnableCell(cells[i].x, cells[i].y);
	}
}

void Gate::Deactivate()
{
	enabled = false;
	for (int i = 0; i < cells.size(); i++) {
		tm.DisableCell(cells[i].x, cells[i].y);
	}
}
