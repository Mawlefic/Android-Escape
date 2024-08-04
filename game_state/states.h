#pragma once
#include "../entity/player.h"
#include "../entity/tile.h"
#include "../entity/collectible.h"
#include "../entity/textbox.h"
#include "renderer.h"
#include "world.h"

class GSM;
class GameState;
class SplashState;
class SandboxState;

// Don't use STATE_COUNT as GameState, update when new states are added (keeping STATE_COUNT at the end)
enum GStates {
	SPLASH_STATE, SANDBOX_STATE, END_STATE, STATE_COUNT
};
const int TOTAL_STATE = STATE_COUNT;

class GSM {
private:
	GameState* states[TOTAL_STATE];
	int currentState;
	bool changingState = true;
	const float transitionTimer = 1.5f;
	float transitionValue = 0.0f;
	float fadeValue = 1.0f;

public:
	GSM();
	~GSM();
	void SwitchState(int newState);
	void Init(int initialState);
	void Update(float delta);
	void Draw();
	void Load();
	void Unload();
};

class GameState {
protected:
	GSM* gsm;
public:
	GameState(GSM* gsm);
	virtual void Update(float delta) = 0;
	virtual void Draw() = 0;
	virtual void Load() = 0;
	virtual void Unload() = 0;
};

class SplashState : public GameState {
private:
	const char* title = "Android Escape";
	const char* prompt = "Press C to Start!";
	const int size = 50;
public:
	SplashState(GSM* gsm);
	void Update(float delta) override;
	void Draw() override;
	void Load() override;
	void Unload() override;
};

class EndState : public GameState {
private:
	const char* prompt = "Thanks for playing!";
	const int size = 50;
public:
	EndState(GSM* gsm);
	void Update(float delta) override;
	void Draw() override;
	void Load() override;
	void Unload() override;
};

class SandboxState : public GameState {
private:
	// entity data
	bool fBossDefeated = false;
	std::vector<MovingTile> mTiles;
	std::vector<Projectile> playerProjectiles;
	std::vector<Projectile> enemyProjectiles;
	std::vector<Collectible> collectibles;
	EnemyList enemies;
	TileMap tileMap;
	TextBox tb;
	Gate bossGate{ tileMap };
	Gate bossEntry{ tileMap };
	Gate mBossGate{ tileMap };
	Gate mBossEntry{ tileMap };
	Camera2D camera;
	Player player;
	int pX, pY;

	// render & world data
	Renderer renderer{};
	World world{};

public:
	SandboxState(GSM* gsm);
	void Update(float delta) override;
	void Draw() override;
	void Load() override;
	void Unload() override;
	void AddMovingPlatform(int x, int y, int valX, int valY, float time, float waitTime, bool isOffset);
	void LoadEntityData(const char* mapPath);
};