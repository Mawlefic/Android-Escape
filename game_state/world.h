#pragma once
#include "../entity/entity.h"
#include "../entity/tile.h"
#include "../entity/player.h"
#include "../entity/projectile.h"
#include "../entity/enemy.h"
#include "../entity/collectible.h"
#include "../entity/textbox.h"
#include <vector>

enum GAME_SOUNDS {
	EXPLOSION, BOSS_EXPLOSION, SHOOT, TOTAL_SOUNDS
};

// handles entity updates and interactions
class World {
private:
	// variables
	// add variable for current state?
	Player* player;
	bool isPaused = false;
	bool oneTimePrompt = true;
	std::vector<MovingTile>* movingTiles;
	std::vector<Projectile>* playerProjectiles;
	std::vector<Projectile>* enemyProjectiles;
	std::vector<Collectible>* collectibles;
	TextBox* text;
	Gate* bossGate;
	Gate* bossEntry;
	Gate* mBossGate;
	Gate* mBossEntry;
	EnemyList backup;
	EnemyList* enemies;
	TileMap* tilemap;
	Sound sounds[TOTAL_SOUNDS];
	Music bgm;

	// world functions
	void SpawnPlayerProjectile(int x, int y, float vx, float vy, float gravity);
	void SpawnHomingEnemyProjectile(int x, int y, float speed, int damage, float lifetime);
	void SpawnHomingEnemyProjectile(int x, int y, float speed, int damage, float lifetime, int size);
	void SpawnTargetedEnemyProjectile(int x, int y, float speed, int damage);
	void SpawnEnemyProjectile(int x, int y, float vx, float vy, float gravity, int damage);
	void SpawnSprayProjectile(int x, int y, float speed, int damage);
	void SpawnTrappingShots(int x, int y, float speed, int damage);
	void RemoveInactiveProjectiles();
	void ResetEnemyPosition();

	// player input handling
	void HandlePlayerInput(float delta);
	void ProcessStandInput(float delta);
	void ProcessJumpInput(float delta);
	void ProcessFallInput(float delta);
	void ProcessDashInput(float delta);

	// player-handler functions
	void MovePlayer(float delta);
	void MoveX(float amount);
	void MoveY(float amount);
	void MoveFlatX(int amount);
	void MoveFlatY(int amount);
	int GetCollisionDistX(int maxDistance);
	int GetCollisionDistY(int maxDistance);
	void HandlePlayerCollision();
	void PlayerCheckGrounded();
	bool TouchingWall(bool wallDir);
	inline int GetCell(int position);

	// enmey handling
	void EnterBossZone();
	void EnterMinibossZone();
	void OnBossDefeat();
	void OnMinibossDefeat();
	void CheckBossSpawn();

	// other
	void HandleUpgrade(CollectType ct);
	void HandleProjectiles(float delta);

public:
	World();
	void Init(TileMap& tm, Player& p, std::vector<MovingTile>& mt, std::vector<Projectile>& pr, std::vector<Projectile>& epr, EnemyList& foes);
	void SetGatePointers(Gate& g, Gate& g2, Gate& g3, Gate& g4);
	void SetCollectibles(std::vector<Collectible>& c);
	void SetTextbox(TextBox& tb);
	void Update(float delta);
};