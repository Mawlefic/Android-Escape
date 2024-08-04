#pragma once
#include "entity.h"
#include "player.h"
#include "collision_object.h"

class Enemy : public Entity
{
protected:
	Player* target;
	int detectionRadius;

public:
	// variables
	CollisionComponent cc;
	bool spottedPlayer;
	int MAX_HP, HP;

	// functions
	virtual void Update(float delta) = 0;
	virtual void Draw();
	virtual void HandlePlayerDetection();
	void SetTarget(Player& player);
	void SetPosition(int x, int y);
};

class Sentry : public Enemy {
private:
	// variables
	const float shotCD = 2.0f;
	float shotCounter = 0.0f;

public:
	// variables
	const int PROJ_DMG = 5;
	const float BULLET_SPEED = 300.0f;
	bool canShoot = true;
	bool facingRight;
	bool isHoming = false;

	// functions
	Sentry();
	Sentry(int detectionRadius);
	Sentry& operator=(const Sentry& s);
	void Update(float delta) override;
	void HandlePlayerDetection() override;
	void TimerUpdate(float delta);
	void Draw() override;
};

class Floater : public Enemy {
public:
	// variables
	const float SPEED = 100.0f;
	const int CONTACT_DMG = 15;
	const int PROJ_DMG = 10;
	float vx, vy, xRemainder, yRemainder;

	// shooting variant
	bool canShoot = false;
	const float shotCD = 1.0f;
	const float PROJ_SPEED = 300.0f;
	float shotCounter = 0.0f;

	// functions
	Floater();
	Floater(int detectionRadius);
	Floater& operator=(const Floater& s);
	void Draw() override;
	void Update(float delta) override;
};

enum Pattern {
	ATK1, ATK2, ATK3
};

class Boss {
private:
	void GetNextPattern();
	void ProcessPattern(float delta);

public:
	// attributes
	CollisionComponent cc;
	float SPEED = 200.0f;
	float xRemainder = 0.0f;
	int leftX, middleX, rightX, destX;
	Pattern attack;
	bool enabled = false;
	bool defeated = false;
	bool isMoving = false;
	const float patternDuration = 10.0f;
	float patternCounter = 0.0f;

	// spawn time
	float spawnTime = 0.0f;

	// pattern 2 
	const float targetedCD = 1.0f;
	const float sprayCD = 0.75f;
	const float targetedSpeed = 400.0f;
	const float spraySpeed = 300.0f;
	float targetedCounter = 0.0f;
	float sprayCounter = 0.0f;

	// pattern 3
	const float trapCD = 0.6f;
	const float homingCD = 1.2f;
	float trapCounter = 0.0f;
	float homingCounter = 0.0f;

	// stat attributes
	const int PROJ_DMG = 10;
	const int MAX_HP = 2000;
	int HP = MAX_HP;

	// functions
	Boss();
	Boss& operator=(const Boss& b);
	void Update(float delta);
	void SetPosition(int x, int y);
	void Draw();
};

class Miniboss {
public:
	// attributes
	CollisionComponent cc;
	bool enabled = false;
	bool defeated = false;
	bool direction = false;

	// attack values
	float shotCD = 0.5f;
	float shotCounter = 0.0f;
	float secondaryShotCD = 1.0f;
	float secondaryShotCounter = 0.0f;
	float tertiaryShotCD = 0.8f;
	float tertiaryShotCounter = 0.0f;

	// spawn time
	float spawnTime = 0.0f;

	// stat attributes
	const float SPEED = 100.0f;
	float xRemainder = 0.0f;
	float PROJ_SPEED = 350.0f;
	const int CONTACT_DMG = 30;
	const int PROJ_DMG = 10;
	const int MAX_HP = 2500;
	int HP = MAX_HP;

	// functions
	Miniboss();
	Miniboss& operator=(const Miniboss& m);
	void Update(float delta);
	void SetPosition(int x, int y);
	void Draw();

};

class EnemyList {
public:
	std::vector<Sentry> sentries;
	std::vector<Floater> floaters;
	Boss boss{};
	Miniboss mBoss{};
	// add other enemies

	EnemyList& operator=(const EnemyList& e) {
		sentries.clear();
		floaters.clear();
		for (int i = 0; i < e.sentries.size(); i++) {
			sentries.push_back(e.sentries[i]);
		}
		for (int i = 0; i < e.floaters.size(); i++) {
			floaters.push_back(e.floaters[i]);
		}
		// miniboss
		mBoss = e.mBoss;
		boss = e.boss;
		return *this;
	}
	
	void addSentry(Sentry& s) {
		sentries.push_back(s);
	}

	void addFloater(Floater& f) {
		floaters.push_back(f);
	}
};