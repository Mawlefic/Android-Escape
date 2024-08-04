#pragma once
#include "../entity/entity.h"
#include "../entity/tile.h"
#include "../entity/player.h"
#include "../entity/projectile.h"
#include "../entity/enemy.h"
#include "../entity/collectible.h"
#include "../entity/textbox.h"
#include <vector>

enum TEXTURES {
	TILESET, TILEMAP, BATTERY, PLUG, DASHING, LASER, BULLET, JETPACK, PLAYER, COMPANION, SENTRY, DROPPER, FINAL_BOSS, TOTAL_TEXTURES
};

// handles drawing to screen
class Renderer {
private:
	// add variable for current state?
	Player* player;
	EnemyList* enemies;
	std::vector<MovingTile>* movingTiles;
	std::vector<Projectile>* playerProjectiles;
	std::vector<Projectile>* enemyProjectiles;
	std::vector<Collectible>* collectibles;
	TextBox* text;
	Gate* bossGate;
	Gate* bossEntry;
	Gate* mBossGate;
	Gate* mBossEntry;
	TileMap* tilemap;
	Camera2D* camera;
	float offset[2]{ 0 };
	float mapWidth = 0.0f;
	float mapHeight = 0.0f;

	// shader + uniform locations
	Shader mapShader;
	int setLoc, mapLoc, widthLoc, heightLoc, offsetLoc, screenLoc;
	Texture2D textures[TOTAL_TEXTURES];

public:
	Renderer();
	~Renderer();
	void Init(TileMap& tm, Camera2D& cam, Player& p, std::vector<MovingTile>& mt, std::vector<Projectile>& pr, std::vector<Projectile>& epr, EnemyList& foes);
	void SetMap(const char* newMapPath);
	void SetGatePointers(Gate& g, Gate& g2, Gate& g3, Gate& g4);
	void SetCollectibles(std::vector<Collectible>& c);
	void SetTextbox(TextBox& tb);
	void DrawTileMap();
	void DrawPlayer();
	void DrawGates();
	void Render();
	void Load();
	void Unload();
};